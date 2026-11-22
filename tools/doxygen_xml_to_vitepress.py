#!/usr/bin/env python3
"""Convert Doxygen XML output into cross-linked VitePress Markdown pages."""

import json
import re
import shutil
import sys
import xml.etree.ElementTree as ET
from pathlib import Path
from typing import Dict, List, Optional, Tuple


def slugify(name: str) -> str:
    s = name.replace("ftxui::", "")
    s = re.sub(r"[^a-zA-Z0-9]+", "-", s).strip("-").lower()
    return s or "item"


def md_escape_inline(text: str) -> str:
    return text.replace("<", "&lt;").replace(">", "&gt;")


def xml_text_plain(elem: Optional[ET.Element]) -> str:
    if elem is None:
        return ""
    return "".join(elem.itertext()).strip()


def first_sentence(text: str) -> str:
    """Extract a clean single-line summary for tables and lists."""
    if not text:
        return ""
    # Take first paragraph only
    para = text.split("\n\n")[0].replace("\n", " ").strip()
    para = re.sub(r"\s+", " ", para)
    # Escape pipes for Markdown tables
    return para.replace("|", "\\|")


class DoxygenToVitePress:
    def __init__(self, xml_dir: Path, out_dir: Path):
        self.xml_dir = xml_dir
        self.out_dir = out_dir
        self.api_dir = out_dir / "api"
        self.class_dir = self.api_dir / "class"
        self.examples_dir = out_dir / "examples"

        # refid -> (url_path, display_name)
        self.ref_map: Dict[str, Tuple[str, str]] = {}
        # short or qualified name -> url_path (for auto-linking types, @see, & guides)
        self.symbol_map: Dict[str, str] = {}
        # refid -> sorted list of (example_label, example_url)
        self.examples_for_refid: Dict[str, List[Tuple[str, str]]] = {}
        # symbol_name -> sorted list of (example_label, example_url)
        self.examples_for_symbol: Dict[str, List[Tuple[str, str]]] = {}
        # header include path (e.g. "ftxui/component/app.hpp") -> url_path
        self.header_map: Dict[str, str] = {
            "ftxui/dom/elements.hpp": "/api/dom",
            "ftxui/component/component.hpp": "/api/component",
            "ftxui/screen/screen.hpp": "/api/class/screen",
            "ftxui/screen/string.hpp": "/api/screen",
            "ftxui/screen/terminal.hpp": "/api/screen",
        }
        # public compounds: refid -> ET.Element (compounddef)
        self.public_classes: Dict[str, ET.Element] = {}
        self.groups: Dict[str, ET.Element] = {}
        # module ("dom"|"component"|"screen") -> {"func": [...], "enum": [...], "typedef": [...]}
        self.module_members: Dict[str, Dict[str, List[ET.Element]]] = {
            "dom": {"func": [], "enum": [], "typedef": []},
            "component": {"func": [], "enum": [], "typedef": []},
            "screen": {"func": [], "enum": [], "typedef": []},
        }
        self.member_anchors: Dict[Tuple[str, str], str] = {}
        self._symbol_regex: Optional[re.Pattern] = None

    def is_public_header(self, loc: Optional[ET.Element]) -> bool:
        if loc is None:
            return False
        file_attr = loc.attrib.get("file", "")
        return file_attr.startswith("include/ftxui/")

    def header_include_path(self, loc: Optional[ET.Element]) -> str:
        if loc is None:
            return ""
        f = loc.attrib.get("file", "")
        if f.startswith("include/"):
            return f[len("include/"):]
        return f

    def _register_symbol_name(self, name: str, url: str) -> None:
        if not name or len(name) < 3:
            return
        # Prefer shorter / primary URLs (classes over random members)
        if name not in self.symbol_map:
            self.symbol_map[name] = url
        short = name.replace("ftxui::", "")
        if short and "::" not in short and short not in self.symbol_map:
            self.symbol_map[short] = url

    def load_all(self) -> None:
        index_tree = ET.parse(self.xml_dir / "index.xml")
        root = index_tree.getroot()

        for comp in root.findall("compound"):
            kind = comp.attrib["kind"]
            refid = comp.attrib["refid"]
            name = comp.findtext("name", "")

            if kind == "group" and refid in ("group__dom", "group__component", "group__screen"):
                cdef = self._load_compounddef(refid)
                if cdef is not None:
                    self.groups[refid] = cdef
                    group_slug = refid.replace("group__", "")
                    self.ref_map[refid] = (f"/api/{group_slug}", name)
                continue

            if kind in ("class", "struct") and name.startswith("ftxui::"):
                if any(skip in name for skip in ("::Internal", "::Private", "::Impl", "box_helper", "flexbox_helper")):
                    continue
                cdef = self._load_compounddef(refid)
                if cdef is None:
                    continue
                if cdef.attrib.get("prot", "public") != "public":
                    continue
                if not self.is_public_header(cdef.find("location")):
                    continue
                self.public_classes[refid] = cdef
                slug = slugify(name)
                url = f"/api/class/{slug}"
                self.ref_map[refid] = (url, name)
                self._register_symbol_name(name, url)
                inc = self.header_include_path(cdef.find("location"))
                if inc and inc not in self.header_map:
                    self.header_map[inc] = url

        # Collect ALL public functions/enums/typedefs from groups + namespaces,
        # classified strictly by their header directory: include/ftxui/<dir>/
        seen_member_ids = set()
        xml_sources = [
            "group__dom.xml",
            "group__component.xml",
            "group__screen.xml",
            "namespaceftxui.xml",
            "namespaceftxui_1_1Container.xml",
            "namespaceftxui_1_1Dimension.xml",
            "namespaceftxui_1_1Terminal.xml",
            "namespaceftxui_1_1animation.xml",
            "namespaceftxui_1_1animation_1_1easing.xml",
        ]
        for xml_name in xml_sources:
            xml_path = self.xml_dir / xml_name
            if not xml_path.exists():
                continue
            cdef = ET.parse(xml_path).getroot().find("compounddef")
            if cdef is None:
                continue
            for sec in cdef.findall("sectiondef"):
                for m in sec.findall("memberdef"):
                    m_id = m.attrib.get("id", "")
                    if not m_id or m_id in seen_member_ids:
                        continue
                    kind = m.attrib.get("kind", "")
                    if kind not in ("function", "enum", "typedef"):
                        continue
                    loc = m.find("location")
                    if loc is None:
                        continue
                    hdr = loc.attrib.get("declfile") or loc.attrib.get("file", "")
                    if not hdr.startswith("include/ftxui/"):
                        continue
                    if any(skip in hdr for skip in ("deprecated.hpp", "take_any_args.hpp", "receiver.hpp")):
                        continue
                    mod = hdr.split("/")[2]
                    if mod not in self.module_members:
                        continue
                    m_name = m.findtext("name", "")
                    if not m_name or m_name.startswith("Reserved"):
                        continue
                    # Only expose documented functions in module pages / API index
                    if kind == "function":
                        has_doc = bool(
                            xml_text_plain(m.find("briefdescription"))
                            or xml_text_plain(m.find("detaileddescription"))
                        )
                        if not has_doc:
                            continue
                    seen_member_ids.add(m_id)
                    bucket = {"function": "func", "enum": "enum", "typedef": "typedef"}[kind]
                    self.module_members[mod][bucket].append(m)

        # Sort each module's members alphabetically by display name
        for mod in ("dom", "component", "screen"):
            for bucket in ("func", "enum", "typedef"):
                self.module_members[mod][bucket].sort(
                    key=lambda m: (m.findtext("qualifiedname", "").replace("ftxui::", "").lower(), m.findtext("name", "").lower())
                )

        # Register member anchors for cross-linking
        for refid, cdef in self.public_classes.items():
            base_url, cname = self.ref_map[refid]
            self._register_members(refid, cdef, base_url, owner_name=cname, register_short=False)

        for mod in ("dom", "component", "screen"):
            page_refid = f"group__{mod}"
            base_url = f"/api/{mod}"
            self.ref_map[page_refid] = (base_url, f"ftxui/{mod}")
            all_mod_members = (
                self.module_members[mod]["enum"]
                + self.module_members[mod]["typedef"]
                + self.module_members[mod]["func"]
            )
            self._register_member_list(page_refid, all_mod_members, base_url, owner_name="ftxui", register_short=True)

        # Common FTXUI type aliases to auto-link in signatures
        self.symbol_map.setdefault("Element", "/api/dom")
        self.symbol_map.setdefault("Elements", "/api/dom")
        self.symbol_map.setdefault("Decorator", "/api/dom")
        self.symbol_map.setdefault("Component", "/api/class/componentbase")
        self.symbol_map.setdefault("Components", "/api/class/componentbase")
        self.symbol_map.setdefault("ComponentDecorator", "/api/component")
        self.symbol_map.setdefault("ElementDecorator", "/api/dom")

        # Build regex for auto-linking known symbols in signatures (longest first)
        type_names = sorted(
            [k for k in self.symbol_map if "::" not in k and k[0].isupper()],
            key=len,
            reverse=True,
        )
        if type_names:
            self._symbol_regex = re.compile(
                r"\b(" + "|".join(re.escape(n) for n in type_names) + r")\b"
            )

        self._load_examples()

    def _load_compounddef(self, refid: str) -> Optional[ET.Element]:
        path = self.xml_dir / f"{refid}.xml"
        if not path.exists():
            return None
        tree = ET.parse(path)
        return tree.getroot().find("compounddef")

    def _load_examples(self) -> None:
        """Scan Doxygen example XMLs + example sources to build symbol -> examples index."""
        self.example_files: List[Tuple[str, str, str, Path]] = []  # (category, stem, url, src_path)
        for xml_file in sorted(self.xml_dir.glob("examples_2*-example.xml")):
            cdef = ET.parse(xml_file).getroot().find("compounddef")
            if cdef is None:
                continue
            rel_path = cdef.findtext("compoundname", "")  # e.g. examples/component/button.cpp
            if not rel_path.startswith("examples/") or not rel_path.endswith(".cpp"):
                continue
            parts = rel_path.split("/")
            if len(parts) != 3:
                continue
            category = parts[1]  # dom or component
            stem = parts[2][:-4]
            url = f"/examples/{category}/{stem}"
            label = f"{category}/{stem}"
            src_path = Path(rel_path)
            if not src_path.exists():
                continue
            self.example_files.append((category, stem, url, src_path))

            # 1. Exact Doxygen refids referenced in this example
            seen_refs = set()
            for r in cdef.findall(".//ref"):
                rid = r.attrib.get("refid", "")
                if rid and rid not in seen_refs:
                    seen_refs.add(rid)
                    self.examples_for_refid.setdefault(rid, []).append((label, url))

            # 2. Identifier tokens in the example source (catches free functions & classes)
            code = src_path.read_text(encoding="utf-8", errors="ignore")
            # Strip comments & string literals roughly before scanning tokens
            code_clean = re.sub(r"//.*", "", code)
            code_clean = re.sub(r"/\*.*?\*/", "", code_clean, flags=re.DOTALL)
            code_clean = re.sub(r'"(?:\\.|[^"\\])*"', '""', code_clean)
            tokens = set(re.findall(r"\b[A-Za-z_][A-Za-z0-9_]*\b", code_clean))
            for tok in tokens:
                if tok in self.symbol_map:
                    self.examples_for_symbol.setdefault(tok, []).append((label, url))

    def _format_examples_line(self, refid: str, symbol_name: str, limit: int = 12) -> str:
        items: Dict[str, str] = {}
        for label, url in self.examples_for_refid.get(refid, []):
            items[label] = url
        short = symbol_name.replace("ftxui::", "")
        if "::" not in short:
            for label, url in self.examples_for_symbol.get(short, []):
                items.setdefault(label, url)
        if not items:
            return ""
        sorted_items = sorted(items.items())
        links = [f"[`{lbl}`]({url})" for lbl, url in sorted_items[:limit]]
        extra = f" *(+{len(sorted_items) - limit} more)*" if len(sorted_items) > limit else ""
        return "**Examples:** " + ", ".join(links) + extra

    def generate_example_pages(self) -> List[dict]:
        if self.examples_dir.exists():
            shutil.rmtree(self.examples_dir)
        (self.examples_dir / "dom").mkdir(parents=True, exist_ok=True)
        (self.examples_dir / "component").mkdir(parents=True, exist_ok=True)

        dom_items = []
        comp_items = []
        for category, stem, url, src_path in self.example_files:
            code = src_path.read_text(encoding="utf-8").strip()
            demo_key = f"{category}/{stem}"
            md = [
                "---",
                "aside: false",
                "pageClass: example-page",
                "---",
                "",
                f"# Example: `{demo_key}`",
                "",
                f'<FtxuiDemo file="{demo_key}" title="{demo_key}.cpp" height="680px" />',
                "",
                "## Source Code",
                "",
                f"```cpp\n{code}\n```",
                "",
            ]
            (self.examples_dir / category / f"{stem}.md").write_text("\n".join(md), encoding="utf-8")
            entry = {"text": stem, "link": url}
            if category == "dom":
                dom_items.append(entry)
            else:
                comp_items.append(entry)

        # Examples index page with featured showcase + categorized grid
        featured = [
            ("component/homescreen", "Full multi-tab demo (htop, colors, spinner, gauges, compiler, paragraphs)"),
            ("component/gallery", "Showcase of all interactive FTXUI components"),
            ("component/canvas_animated", "Real-time animated braille & block drawing"),
            ("component/window", "Draggable and resizable floating windows"),
            ("component/modal_dialog", "Modal dialog overlay with keyboard & mouse support"),
            ("dom/table", "Styled tables with borders, alignments, and colors"),
        ]
        idx = [
            "# Examples",
            "",
            "Every example below includes an interactive WebAssembly terminal and cross-linked C++ source code.",
            "",
            "## Featured Demos",
            "",
            "| Demo | Description |",
            "| --- | --- |",
            *[f"| [`{key}`](/examples/{key}) | {desc} |" for key, desc in featured],
            "",
            f"## Component Examples ({len(comp_items)})",
            "",
            *[f"- [`{it['text']}`]({it['link']})" for it in comp_items],
            "",
            f"## DOM Examples ({len(dom_items)})",
            "",
            *[f"- [`{it['text']}`]({it['link']})" for it in dom_items],
            "",
        ]
        (self.examples_dir / "index.md").write_text("\n".join(idx), encoding="utf-8")

        return [
            {"text": "Examples Overview", "link": "/examples/"},
            {"text": "Component Examples", "collapsed": False, "items": comp_items},
            {"text": "DOM Examples", "collapsed": False, "items": dom_items},
        ]
        path = self.xml_dir / f"{refid}.xml"
        if not path.exists():
            return None
        tree = ET.parse(path)
        return tree.getroot().find("compounddef")

    def _register_members(
        self, page_refid: str, cdef: ET.Element, base_url: str, owner_name: str, register_short: bool
    ) -> None:
        members: List[ET.Element] = []
        for sec in cdef.findall("sectiondef"):
            for m in sec.findall("memberdef"):
                if m.attrib.get("prot", "public") != "public":
                    continue
                if m.attrib.get("kind") == "friend":
                    continue
                members.append(m)
        self._register_member_list(page_refid, members, base_url, owner_name, register_short)

    def _register_member_list(
        self, page_refid: str, members: List[ET.Element], base_url: str, owner_name: str, register_short: bool
    ) -> None:
        used: Dict[str, int] = {}
        short_owner = owner_name.replace("ftxui::", "")
        for m in members:
            m_id = m.attrib["id"]
            m_name = m.findtext("name", "")
            if not m_name or m_name.startswith("Reserved"):
                continue
            anchor = slugify(m_name)
            if anchor in used:
                used[anchor] += 1
                unique_anchor = f"{anchor}-{used[anchor]}"
            else:
                used[anchor] = 0
                unique_anchor = anchor
            self.member_anchors[(page_refid, m_id)] = unique_anchor
            url = f"{base_url}#{unique_anchor}"
            if m_id not in self.ref_map:
                self.ref_map[m_id] = (url, m_name)
            qname_xml = m.findtext("qualifiedname", "").strip()
            if qname_xml:
                self.symbol_map.setdefault(qname_xml, url)
                self.symbol_map.setdefault(qname_xml.replace("ftxui::", ""), url)
            qname = f"{owner_name}::{m_name}"
            self.symbol_map.setdefault(qname, url)
            if short_owner and short_owner != "ftxui":
                self.symbol_map.setdefault(f"{short_owner}::{m_name}", url)
            if register_short:
                self.symbol_map.setdefault(m_name, url)

    def _autolink_html_types(self, escaped_html: str) -> str:
        """Auto-link known FTXUI type names in already-HTML-escaped plain text."""
        if not self._symbol_regex:
            return escaped_html
        # Clean up Doxygen's "std::function&lt; void()&gt;" spacing
        escaped_html = escaped_html.replace("&lt; ", "&lt;").replace(" &gt;", "&gt;")

        def repl(match: re.Match) -> str:
            sym = match.group(1)
            url = self.symbol_map.get(sym)
            if url:
                return f'<a href="{url}">{sym}</a>'
            return sym

        return self._symbol_regex.sub(repl, escaped_html)

    def render_ref(self, refid: str, text: str) -> str:
        if refid in self.ref_map:
            url, _ = self.ref_map[refid]
            return f"[{md_escape_inline(text)}]({url})"
        clean = text.strip().replace("ftxui::", "")
        if clean in self.symbol_map:
            return f"[{md_escape_inline(text)}]({self.symbol_map[clean]})"
        return md_escape_inline(text)

    def render_type_html(self, elem: Optional[ET.Element]) -> str:
        """Render a C++ type with clickable links inside an HTML code block."""
        if elem is None:
            return ""
        parts: List[str] = []
        if elem.text:
            parts.append(self._autolink_html_types(md_escape_inline(elem.text)))
        for child in elem:
            if child.tag == "ref":
                refid = child.attrib.get("refid", "")
                txt = "".join(child.itertext())
                if refid in self.ref_map:
                    url, _ = self.ref_map[refid]
                    parts.append(f'<a href="{url}">{md_escape_inline(txt)}</a>')
                else:
                    parts.append(self._autolink_html_types(md_escape_inline(txt)))
            else:
                parts.append(self._autolink_html_types(md_escape_inline("".join(child.itertext()))))
            if child.tail:
                parts.append(self._autolink_html_types(md_escape_inline(child.tail)))
        return "".join(parts)

    def render_inline(self, elem: ET.Element) -> str:
        parts: List[str] = []
        if elem.text:
            parts.append(elem.text)
        for child in elem:
            tag = child.tag
            if tag == "ref":
                refid = child.attrib.get("refid", "")
                txt = "".join(child.itertext())
                parts.append(self.render_ref(refid, txt))
            elif tag == "computeroutput":
                txt = "".join(child.itertext())
                parts.append(f"`{txt}`")
            elif tag == "emphasis":
                parts.append(f"*{self.render_inline(child)}*")
            elif tag == "bold":
                parts.append(f"**{self.render_inline(child)}**")
            elif tag == "ulink":
                url = child.attrib.get("url", "")
                txt = "".join(child.itertext()) or url
                parts.append(f"[{txt}]({url})")
            elif tag == "linebreak":
                parts.append("  \n")
            elif tag == "sp":
                parts.append(" ")
            elif tag == "mdash":
                parts.append("--")
            elif tag == "ndash":
                parts.append("–")
            elif tag in ("programlisting", "parameterlist", "simplesect", "itemizedlist", "orderedlist", "heading"):
                parts.append("\n\n" + self.render_block(child) + "\n\n")
            else:
                parts.append(self.render_inline(child))
            if child.tail:
                parts.append(child.tail)
        return "".join(parts)

    def render_programlisting(self, elem: ET.Element) -> str:
        filename = elem.attrib.get("filename", "").lower()
        if filename.endswith((".bash", ".sh", ".txt", ".text")):
            lang = "text"
        elif filename.endswith(".cmake"):
            lang = "cmake"
        else:
            lang = "cpp"

        lines: List[str] = []
        for codeline in elem.findall("codeline"):
            line_parts: List[str] = []
            for node in codeline.iter():
                if node.tag == "sp":
                    line_parts.append(" ")
                else:
                    if node.text:
                        line_parts.append(node.text)
                if node is not codeline and node.tail:
                    line_parts.append(node.tail)
            lines.append("".join(line_parts).rstrip())
        code = "\n".join(lines).strip("\n")

        # Heuristic: box-drawing ASCII art output should be plain text
        if lang == "cpp" and any(ch in code for ch in "┌┐└┘│─╭╮╰╯█▓▒░"):
            lang = "text"

        return f"```{lang}\n{code}\n```"

    def _link_see_also_item(self, text: str) -> str:
        # If already a markdown link, keep it
        if "](" in text:
            return text
        clean = text.strip().rstrip(".")
        key = clean.replace("ftxui::", "").replace("()", "")
        if key in self.symbol_map:
            return f"[`{clean}`]({self.symbol_map[key]})"
        return f"`{clean}`"

    def render_block(self, elem: ET.Element) -> str:
        tag = elem.tag
        if tag == "para":
            txt = self.render_inline(elem).strip()
            if txt in ("Example", "Output"):
                return f"**{txt}**"
            return txt
        if tag == "programlisting":
            return self.render_programlisting(elem)
        if tag == "heading":
            txt = self.render_inline(elem).strip()
            return f"**{txt}**"
        if tag == "itemizedlist":
            items = []
            for li in elem.findall("listitem"):
                txt = self.render_description(li).strip().replace("\n", "\n  ")
                items.append(f"- {txt}")
            return "\n".join(items)
        if tag == "orderedlist":
            items = []
            for idx, li in enumerate(elem.findall("listitem"), 1):
                txt = self.render_description(li).strip().replace("\n", "\n   ")
                items.append(f"{idx}. {txt}")
            return "\n".join(items)
        if tag == "parameterlist":
            kind = elem.attrib.get("kind", "param")
            title = "Parameters" if kind == "param" else kind.capitalize()
            rows = [f"**{title}:**\n"]
            for item in elem.findall("parameteritem"):
                names = [xml_text_plain(n) for n in item.findall(".//parametername")]
                desc = self.render_description(item.find("parameterdescription")).strip()
                rows.append(f"- `{', '.join(names)}`: {desc}")
            return "\n".join(rows)
        if tag == "simplesect":
            kind = elem.attrib.get("kind", "")
            body = self.render_description(elem).strip()
            if kind == "note":
                return f"::: info Note\n{body}\n:::"
            if kind == "warning":
                return f"::: warning Warning\n{body}\n:::"
            if kind == "see":
                return f"__SEE_ALSO__:{self._link_see_also_item(body)}"
            if kind == "return":
                return f"**Returns:** {body}"
            return body
        return self.render_inline(elem).strip()

    def render_description(self, elem: Optional[ET.Element]) -> str:
        if elem is None:
            return ""
        raw_blocks: List[str] = []
        for child in elem:
            rendered = self.render_block(child)
            if rendered:
                raw_blocks.append(rendered)

        # Merge consecutive __SEE_ALSO__ blocks into a single clean line
        merged: List[str] = []
        see_items: List[str] = []

        def flush_see():
            nonlocal see_items
            if see_items:
                # Deduplicate while preserving order
                unique = list(dict.fromkeys(see_items))
                merged.append("**See also:** " + ", ".join(unique))
                see_items = []

        for block in raw_blocks:
            # A <para> may have expanded nested <simplesect kind="see"> lines
            sub_blocks = [b.strip() for b in re.split(r"\n{2,}", block) if b.strip()]
            for sb in sub_blocks:
                if sb.startswith("__SEE_ALSO__:"):
                    see_items.append(sb[len("__SEE_ALSO__:"):])
                else:
                    flush_see()
                    merged.append(sb)
        flush_see()

        text = "\n\n".join(merged)
        text = re.sub(r"\n{3,}", "\n\n", text)
        # Convert Chromium-style |param| references in prose to inline code `param`.
        out_lines: List[str] = []
        in_fence = False
        for line in text.splitlines():
            if line.strip().startswith("```"):
                in_fence = not in_fence
                out_lines.append(line)
                continue
            if not in_fence:
                line = re.sub(r"(?<!`)\|([A-Za-z_][A-Za-z0-9_:]*)\|(?!`)", r"`\1`", line)
            out_lines.append(line)
        return "\n".join(out_lines).strip()

    def render_member_signature_cpp(self, m: ET.Element) -> str:
        kind = m.attrib.get("kind", "")
        name = m.findtext("name", "")
        ret_type = xml_text_plain(m.find("type"))

        prefix = []
        if m.attrib.get("static") == "yes":
            prefix.append("static")
        if m.attrib.get("virt") in ("virtual", "pure-virtual"):
            prefix.append("virtual")
        prefix_str = (" ".join(prefix) + " ") if prefix else ""

        if kind == "function":
            params = []
            for p in m.findall("param"):
                ptype = xml_text_plain(p.find("type"))
                pname = xml_text_plain(p.find("declname"))
                defval = xml_text_plain(p.find("defval"))
                piece = f"{ptype} {pname}".strip()
                if defval:
                    piece += f" = {defval}"
                params.append(piece)
            const_str = " const" if m.attrib.get("const") == "yes" else ""
            pure_str = " = 0" if m.attrib.get("virt") == "pure-virtual" else ""
            ret_prefix = f"{ret_type} " if ret_type else ""
            sig = f"{prefix_str}{ret_prefix}{name}({', '.join(params)}){const_str}{pure_str};"
        elif kind == "variable":
            init = xml_text_plain(m.find("initializer"))
            init_str = f" {init}" if init else ""
            sig = f"{prefix_str}{ret_type} {name}{init_str};"
        elif kind == "typedef":
            sig = f"using {name} = {ret_type};"
        elif kind == "enum":
            vals = [ev.findtext("name", "") for ev in m.findall("enumvalue")]
            sig = f"enum {name} {{ {', '.join(vals)} }};"
        else:
            sig = name

        return f"```cpp\n{sig}\n```"

    def render_member(self, page_refid: str, m: ET.Element) -> str:
        m_id = m.attrib["id"]
        name = m.findtext("name", "")
        anchor = self.member_anchors.get((page_refid, m_id), slugify(name))

        lines = [f"### `{name}` {{#{anchor}}}", ""]
        lines.append(self.render_member_signature_cpp(m))
        lines.append("")

        brief = self.render_description(m.find("briefdescription"))
        detail = self.render_description(m.find("detaileddescription"))
        if brief:
            lines.append(brief)
            lines.append("")
        if detail and detail != brief:
            lines.append(detail)
            lines.append("")

        if m.attrib.get("kind") == "enum":
            ev_lines = []
            for ev in m.findall("enumvalue"):
                ev_name = ev.findtext("name", "")
                ev_desc = self.render_description(ev.find("briefdescription")) or self.render_description(ev.find("detaileddescription"))
                if ev_desc:
                    ev_lines.append(f"- `{ev_name}`: {ev_desc}")
            if ev_lines:
                lines.append("\n".join(ev_lines))
                lines.append("")

        ex_line = self._format_examples_line(m_id, name)
        if ex_line:
            lines.append(ex_line)
            lines.append("")

        return "\n".join(lines)

    def render_summary_table(self, page_refid: str, members: List[ET.Element]) -> str:
        if len(members) < 2:
            return ""
        rows = ["| Symbol | Description |", "| --- | --- |"]
        seen_names = set()
        for m in members:
            m_id = m.attrib["id"]
            name = m.findtext("name", "")
            if name in seen_names:
                continue
            seen_names.add(name)
            anchor = self.member_anchors.get((page_refid, m_id), slugify(name))
            brief = first_sentence(self.render_description(m.find("briefdescription")))
            if not brief:
                brief = first_sentence(self.render_description(m.find("detaileddescription")))
            safe_name = name.replace("|", "\\|")
            safe_brief = brief.replace("|", "\\|")
            rows.append(f"| [`{safe_name}`](#{anchor}) | {safe_brief} |")
        return "\n".join(rows) + "\n"

    def generate_class_page(self, refid: str, cdef: ET.Element) -> Tuple[str, str]:
        name = cdef.findtext("compoundname", "")
        slug = slugify(name)
        inc = self.header_include_path(cdef.find("location"))

        lines = ["---", "pageClass: api-page", "---", "", f"# `{name}`", ""]
        if inc:
            lines.append(f"```cpp\n#include <{inc}>\n```")
            lines.append("")

        bases = cdef.findall("basecompoundref")
        if bases:
            base_links = [
                self.render_ref(b.attrib.get("refid", ""), xml_text_plain(b))
                for b in bases
            ]
            lines.append(f"**Inherits from:** {', '.join(base_links)}")
            lines.append("")

        derived = cdef.findall("derivedcompoundref")
        if derived:
            der_links = [
                self.render_ref(d.attrib.get("refid", ""), xml_text_plain(d))
                for d in derived
                if d.attrib.get("refid", "") in self.ref_map
            ]
            if der_links:
                lines.append(f"**Derived classes:** {', '.join(der_links)}")
                lines.append("")

        brief = self.render_description(cdef.find("briefdescription"))
        detail = self.render_description(cdef.find("detaileddescription"))
        if brief:
            lines.append(brief)
            lines.append("")
        if detail and detail != brief:
            lines.append(detail)
            lines.append("")

        ex_line = self._format_examples_line(refid, name)
        if ex_line:
            lines.append(ex_line)
            lines.append("")

        section_titles = {
            "public-type": "Public Types",
            "public-static-func": "Static Functions",
            "public-func": "Public Member Functions",
            "public-attrib": "Public Attributes",
            "public-static-attrib": "Static Attributes",
        }

        all_public_members: List[ET.Element] = []
        sections_data: List[Tuple[str, List[ET.Element]]] = []
        for sec in cdef.findall("sectiondef"):
            sec_kind = sec.attrib.get("kind", "")
            if sec_kind not in section_titles:
                continue
            members = [
                m for m in sec.findall("memberdef")
                if m.attrib.get("prot", "public") == "public"
                and m.attrib.get("kind") != "friend"
                and not m.findtext("name", "").startswith("Reserved")
            ]
            if members:
                sections_data.append((section_titles[sec_kind], members))
                all_public_members.extend(members)

        if len(all_public_members) >= 4:
            lines.append("## Summary")
            lines.append("")
            lines.append(self.render_summary_table(refid, all_public_members))

        for title, members in sections_data:
            lines.append(f"## {title}")
            lines.append("")
            for m in members:
                lines.append(self.render_member(refid, m))

        (self.class_dir / f"{slug}.md").write_text("\n".join(lines), encoding="utf-8")
        return slug, name

    def generate_module_page(self, mod: str) -> None:
        refid = f"group__{mod}"
        lines = ["---", "pageClass: api-page", "---", "", f"# Module `ftxui/{mod}`", ""]

        # Classes whose header lives in include/ftxui/<mod>/
        # Hide internal plumbing structs from the module overview table
        # (their pages are still generated and reachable via cross-links/search).
        hidden_plumbing = {
            "ftxui::ReceiverImpl",
            "ftxui::SenderImpl",
            "ftxui::CapturedMouseInterface",
            "ftxui::Node::Status",
            "ftxui::AnimationTask",
            "ftxui::Requirement::Focused",
        }
        mod_classes = []
        for c_ref, cdef in self.public_classes.items():
            inc = self.header_include_path(cdef.find("location"))
            if inc.startswith(f"ftxui/{mod}/"):
                url, cname = self.ref_map[c_ref]
                if cname in hidden_plumbing:
                    continue
                brief = first_sentence(self.render_description(cdef.find("briefdescription")))
                mod_classes.append((cname, url, brief))
        if mod_classes:
            lines.append("## Classes & Structs")
            lines.append("")
            lines.append("| Class | Description |")
            lines.append("| --- | --- |")
            for cname, url, brief in sorted(mod_classes):
                lines.append(f"| [`{cname}`]({url}) | {brief} |")
            lines.append("")

        for bucket, sec_title in (("enum", "Enumerations"), ("typedef", "Type Aliases"), ("func", "Functions")):
            members = self.module_members[mod][bucket]
            if not members:
                continue
            lines.append(f"## {sec_title}")
            lines.append("")
            if bucket == "func":
                lines.append(self.render_summary_table(refid, members))
            for m in members:
                lines.append(self.render_member(refid, m))

        (self.api_dir / f"{mod}.md").write_text("\n".join(lines), encoding="utf-8")

    def generate_namespace_page(self, refid: str, cdef: ET.Element) -> Tuple[str, str]:
        name = cdef.findtext("compoundname", "")
        slug = slugify(name)
        lines = [f"# Namespace `{name}`", ""]

        brief = self.render_description(cdef.find("briefdescription"))
        detail = self.render_description(cdef.find("detaileddescription"))
        if brief:
            lines.append(brief)
            lines.append("")
        if detail and detail != brief:
            lines.append(detail)
            lines.append("")

        for sec in cdef.findall("sectiondef"):
            sec_kind = sec.attrib.get("kind", "")
            if sec_kind not in ("func", "enum", "typedef", "var"):
                continue
            members = [
                m for m in sec.findall("memberdef")
                if not m.findtext("name", "").startswith("Reserved")
            ]
            if not members:
                continue
            sec_title = {"func": "Functions", "enum": "Enumerations", "typedef": "Type Aliases", "var": "Variables"}[sec_kind]
            lines.append(f"## {sec_title}")
            lines.append("")
            if sec_kind == "func":
                lines.append(self.render_summary_table(refid, members))
            for m in members:
                lines.append(self.render_member(refid, m))

        (self.ns_dir / f"{slug}.md").write_text("\n".join(lines), encoding="utf-8")
        return slug, name

    def autolink_markdown_guides(self) -> int:
        """Auto-link `ftxui::...` symbols and header paths in guide/install Markdown."""
        count = 0
        # Match inline code `ftxui::...` NOT already inside a markdown link [`...`](...)
        sym_pattern = re.compile(r"(?<!\[)`(ftxui::[A-Za-z0-9_:]+(?:\([^`]*\))?)`(?!\])")
        # Match quoted header paths like "ftxui/component/component.hpp" (idempotent)
        hdr_pattern = re.compile(r'(?<!`)"(ftxui/[a-zA-Z0-9_/]+\.hpp)"(?!`)')

        for folder in (self.out_dir / "guide", self.out_dir / "install"):
            if not folder.exists():
                continue
            for md_file in folder.glob("*.md"):
                lines = md_file.read_text(encoding="utf-8").splitlines()
                out_lines: List[str] = []
                in_fence = False
                changed = False
                for line in lines:
                    stripped = line.lstrip()
                    if stripped.startswith("```"):
                        in_fence = not in_fence
                        out_lines.append(line)
                        continue
                    if in_fence or stripped.startswith("#"):
                        out_lines.append(line)
                        continue

                    def repl_sym(m: re.Match) -> str:
                        nonlocal count, changed
                        full = m.group(1)
                        base = full.split("(", 1)[0]
                        url = self.symbol_map.get(base) or self.symbol_map.get(base.replace("ftxui::", ""))
                        if url:
                            count += 1
                            changed = True
                            return f"[`{full}`]({url})"
                        return m.group(0)

                    def repl_hdr(m: re.Match) -> str:
                        nonlocal count, changed
                        hdr = m.group(1)
                        url = self.header_map.get(hdr)
                        if url:
                            count += 1
                            changed = True
                            return f'[`"{hdr}"`]({url})'
                        return m.group(0)

                    new_line = sym_pattern.sub(repl_sym, line)
                    new_line = hdr_pattern.sub(repl_hdr, new_line)
                    out_lines.append(new_line)

                if changed:
                    md_file.write_text("\n".join(out_lines) + "\n", encoding="utf-8")
        return count

    def run(self) -> None:
        if self.api_dir.exists():
            shutil.rmtree(self.api_dir)
        self.class_dir.mkdir(parents=True, exist_ok=True)

        self.load_all()

        class_entries: List[Tuple[str, str]] = []
        for refid, cdef in sorted(self.public_classes.items(), key=lambda kv: kv[1].findtext("compoundname", "")):
            class_entries.append(self.generate_class_page(refid, cdef))

        for mod in ("dom", "component", "screen"):
            self.generate_module_page(mod)

        # Collect unique function lists classified strictly by include/ftxui/<dir>/
        def unique_module_funcs(mod: str) -> List[Tuple[str, str, str]]:
            page_refid = f"group__{mod}"
            base_url = f"/api/{mod}"
            seen = set()
            result = []
            for m in self.module_members[mod]["func"]:
                m_id = m.attrib["id"]
                m_name = m.findtext("name", "")
                if not m_name or m_name.startswith("operator") or m_name.startswith("Reserved"):
                    continue
                qname = m.findtext("qualifiedname", "").replace("ftxui::", "")
                display = qname if "::" in qname else m_name
                if display in seen:
                    continue
                seen.add(display)
                anchor = self.member_anchors.get((page_refid, m_id), slugify(m_name))
                brief = first_sentence(self.render_description(m.find("briefdescription")))
                if not brief:
                    brief = first_sentence(self.render_description(m.find("detaileddescription")))
                result.append((display, f"{base_url}#{anchor}", brief))
            return sorted(result, key=lambda x: x[0].lower())

        dom_funcs = unique_module_funcs("dom")
        comp_funcs = unique_module_funcs("component")
        screen_funcs = unique_module_funcs("screen")

        # Helper to render logically grouped sections with collapsed variants
        def render_grouped_list(
            funcs: List[Tuple[str, str, str]],
            groups_spec: List[Tuple[str, List[Tuple[str, List[str]]]]],
        ) -> List[str]:
            by_name = {name: (url, brief) for name, url, brief in funcs}
            used = set()
            out: List[str] = []
            for section_title, families in groups_spec:
                sec_lines: List[str] = []
                for primary, variants in families:
                    present_primary = primary in by_name
                    present_variants = [v for v in variants if v in by_name]
                    if not present_primary and not present_variants:
                        continue
                    if present_primary:
                        url, brief = by_name[primary]
                        used.add(primary)
                        line = f"- [`{primary}`]({url})"
                        if present_variants:
                            vlinks = ", ".join(f"[`{v}`]({by_name[v][0]})" for v in present_variants)
                            used.update(present_variants)
                            line += f" *(variants: {vlinks})*"
                        if brief:
                            line += f": {brief}"
                        sec_lines.append(line)
                    else:
                        first_v = present_variants[0]
                        url, brief = by_name[first_v]
                        used.update(present_variants)
                        vlinks = ", ".join(f"[`{v}`]({by_name[v][0]})" for v in present_variants)
                        sec_lines.append(f"- {vlinks}" + (f": {brief}" if brief else ""))
                if sec_lines:
                    out.append(f"### {section_title}")
                    out.append("")
                    out.extend(sec_lines)
                    out.append("")

            leftovers = [(n, u, b) for n, u, b in funcs if n not in used]
            if leftovers:
                out.append("### Other")
                out.append("")
                for n, u, b in leftovers:
                    out.append(f"- [`{n}`]({u})" + (f": {b}" if b else ""))
                out.append("")
            return out

        dom_groups = [
            ("Text & Paragraphs", [
                ("text", ["vtext"]),
                ("paragraph", ["paragraphAlignLeft", "paragraphAlignRight", "paragraphAlignCenter", "paragraphAlignJustify"]),
            ]),
            ("Layout & Containers", [
                ("hbox", []),
                ("vbox", []),
                ("dbox", []),
                ("flexbox", ["hflow", "vflow"]),
                ("gridbox", []),
                ("filler", []),
                ("emptyElement", []),
            ]),
            ("Borders, Windows & Separators", [
                ("border", ["borderLight", "borderDashed", "borderHeavy", "borderDouble", "borderRounded", "borderEmpty", "borderStyled", "borderWith"]),
                ("window", []),
                ("automerge", []),
                ("separator", ["separatorLight", "separatorDashed", "separatorHeavy", "separatorDouble", "separatorEmpty", "separatorStyled", "separatorCharacter", "separatorHSelector", "separatorVSelector"]),
            ]),
            ("Data Visualization & Drawing", [
                ("gauge", ["gaugeLeft", "gaugeRight", "gaugeUp", "gaugeDown", "gaugeDirection", "gaugeCharset"]),
                ("graph", []),
                ("spinner", []),
                ("canvas", []),
            ]),
            ("Styling & Color", [
                ("color", ["bgcolor"]),
                ("bold", []),
                ("dim", []),
                ("italic", []),
                ("inverted", []),
                ("underlined", ["underlinedDouble"]),
                ("blink", []),
                ("strikethrough", []),
                ("hyperlink", []),
                ("selectionColor", ["selectionForegroundColor", "selectionBackgroundColor", "selectionStyle", "selectionStyleReset"]),
            ]),
            ("Flexibility & Sizing", [
                ("flex", ["flex_grow", "flex_shrink", "flex_factor", "flex_grow_factor", "flex_shrink_factor"]),
                ("xflex", ["xflex_grow", "xflex_shrink", "xflex_factor", "xflex_grow_factor", "xflex_shrink_factor"]),
                ("yflex", ["yflex_grow", "yflex_shrink", "yflex_factor", "yflex_grow_factor", "yflex_shrink_factor"]),
                ("notflex", []),
                ("size", []),
            ]),
            ("Alignment, Layering & Utilities", [
                ("center", ["hcenter", "vcenter", "align_right"]),
                ("clear_under", []),
                ("reflect", []),
                ("nothing", []),
                ("Render", []),
                ("Dimension::Fit", []),
            ]),
            ("Scrolling & Focus", [
                ("frame", ["xframe", "yframe"]),
                ("vscroll_indicator", ["hscroll_indicator"]),
                ("focus", ["select", "focusPosition", "focusPositionRelative"]),
                ("focusCursorBlock", ["focusCursorBlockBlinking", "focusCursorBar", "focusCursorBarBlinking", "focusCursorUnderline", "focusCursorUnderlineBlinking"]),
            ]),
        ]

        comp_groups = [
            ("Interactive Controls", [
                ("Button", []),
                ("Checkbox", []),
                ("Input", []),
                ("Menu", ["MenuEntry"]),
                ("Radiobox", []),
                ("Dropdown", []),
                ("Toggle", []),
                ("Slider", []),
            ]),
            ("Containers & Splits", [
                ("Container::Vertical", ["Container::Horizontal", "Container::Tab", "Container::Stacked"]),
                ("ResizableSplit", ["ResizableSplitLeft", "ResizableSplitRight", "ResizableSplitTop", "ResizableSplitBottom"]),
            ]),
            ("Composition & Overlays", [
                ("Renderer", []),
                ("CatchEvent", []),
                ("Maybe", []),
                ("Modal", []),
                ("Collapsible", []),
                ("Hoverable", []),
                ("Window", []),
            ]),
            ("Animation", [
                ("animation::RequestAnimationFrame", []),
            ]),
        ]

        # Separate core user-facing classes from parameter-pack Option/State & helper structs.
        # Parameter-pack structs still get generated pages (accessible via function cross-links
        # and search), but don't clutter the top-level API overview or sidebar.
        def is_core_class(cname: str) -> bool:
            short = cname.replace("ftxui::", "")
            # Exclude nested helper structs
            if "::" in short:
                return False
            # Exclude Option/State parameter packs and internal/utility wrappers
            if short.endswith(("Option", "Options", "State", "Task", "Interface", "Impl")):
                return False
            if short in (
                "AutoReset",
                "ConstRef",
                "Ref",
                "StringRef",
                "ConstStringRef",
                "ConstStringListRef",
                "ColorInfo",
                "Dimensions",
            ):
                return False
            return True

        core_class_entries = [(s, n) for s, n in class_entries if is_core_class(n)]

        screen_core = [
            ("ftxui::Screen", "/api/class/screen", "A rectangular grid of `Cell`s rendered to the terminal."),
            ("ftxui::Surface", "/api/class/surface", "Base 2D grid of `Cell`s (formerly `Image`)."),
            ("ftxui::Cell", "/api/class/cell", "A single Unicode grapheme and its foreground/background style (formerly `Pixel`)."),
            ("ftxui::Color", "/api/class/color", "Terminal color supporting Palette16, Palette256, and 24-bit TrueColor (RGB/HSV)."),
            ("ftxui::Box", "/api/class/box", "Inclusive 2D bounding rectangle (`x_min`, `x_max`, `y_min`, `y_max`)."),
        ]

        # API index page: Screen -> Elements -> Components -> Core Classes
        idx_lines = [
            "# API Reference",
            "",
            "FTXUI is layered into three modules: **Screen** (`ftxui/screen`), **Elements** (`ftxui/dom`), and **Components** (`ftxui/component`).",
            "",
            "## Screen (`ftxui/screen`)",
            "",
            *[f"- [`{name}`]({url}): {desc}" for name, url, desc in screen_core],
            "",
            "*(Additional terminal & string utilities are available in the [`ftxui/screen` module reference](/api/screen).)*",
            "",
            "## Elements (`ftxui/dom`)",
            "",
            *render_grouped_list(dom_funcs, dom_groups),
            "## Components (`ftxui/component`)",
            "",
            *render_grouped_list(comp_funcs, comp_groups),
            "## Core Classes",
            "",
            "*(Component option/state structs like `ButtonOption`, `MenuOption`, `InputOption`, `WindowOptions`, etc. are linked directly from the functions that accept them.)*",
            "",
            "| Class | Description |",
            "| --- | --- |",
        ]
        for slug, cname in core_class_entries:
            refid = next(r for r, c in self.public_classes.items() if c.findtext("compoundname") == cname)
            brief = first_sentence(self.render_description(self.public_classes[refid].find("briefdescription")))
            idx_lines.append(f"| [`{cname}`](/api/class/{slug}) | {brief} |")
        (self.api_dir / "index.md").write_text("\n".join(idx_lines) + "\n", encoding="utf-8")

        # Write VitePress sidebar JSON: Screen -> Elements -> Components -> Core Classes
        sidebar = [
            {
                "text": "API Overview",
                "link": "/api/",
            },
            {
                "text": "Screen",
                "collapsed": False,
                "items": [
                    {"text": "Screen", "link": "/api/class/screen"},
                    {"text": "Surface", "link": "/api/class/surface"},
                    {"text": "Cell", "link": "/api/class/cell"},
                    {"text": "Color", "link": "/api/class/color"},
                    {"text": "Box", "link": "/api/class/box"},
                    {"text": "All Screen Utilities", "link": "/api/screen"},
                ],
            },
            {
                "text": "Elements (DOM)",
                "collapsed": False,
                "items": [
                    {"text": "All Elements", "link": "/api/dom"},
                    *[{"text": name, "link": url} for name, url, _ in dom_funcs],
                ],
            },
            {
                "text": "Components",
                "collapsed": False,
                "items": [
                    {"text": "All Components", "link": "/api/component"},
                    *[{"text": name, "link": url} for name, url, _ in comp_funcs],
                ],
            },
            {
                "text": "Core Classes",
                "collapsed": True,
                "items": [
                    *[
                        {"text": cname.replace("ftxui::", ""), "link": f"/api/class/{slug}"}
                        for slug, cname in core_class_entries
                    ],
                ],
            },
        ]
        vp_dir = self.out_dir / ".vitepress"
        vp_dir.mkdir(parents=True, exist_ok=True)
        (vp_dir / "api_sidebar.json").write_text(json.dumps(sidebar, indent=2) + "\n", encoding="utf-8")

        examples_sidebar = self.generate_example_pages()
        (vp_dir / "examples_sidebar.json").write_text(json.dumps(examples_sidebar, indent=2) + "\n", encoding="utf-8")

        # Export symbol & header map for Shiki code-block cross-linking
        code_symbols: Dict[str, str] = {}
        # 1. All public classes/structs
        for refid, (url, cname) in self.ref_map.items():
            if refid in self.public_classes:
                short = cname.replace("ftxui::", "")
                if "::" not in short:
                    code_symbols[short] = url
        # 2. All free functions / enums in dom, component, screen groups
        for refid, cdef in self.groups.items():
            for sec in cdef.findall("sectiondef"):
                for m in sec.findall("memberdef"):
                    m_id = m.attrib["id"]
                    m_name = m.findtext("name", "")
                    if m_id in self.ref_map and m_name and m_name.isidentifier() and len(m_name) >= 3:
                        code_symbols.setdefault(m_name, self.ref_map[m_id][0])
        # 3. Unambiguous PascalCase methods on key classes (App::Loop, App::TerminalOutput, Screen::Create, etc.)
        for refid, cdef in self.public_classes.items():
            for sec in cdef.findall("sectiondef"):
                for m in sec.findall("memberdef"):
                    m_id = m.attrib["id"]
                    m_name = m.findtext("name", "")
                    if (
                        m_id in self.ref_map
                        and m_name
                        and m_name[0].isupper()
                        and m_name.isidentifier()
                        and not m_name.startswith("Reserved")
                    ):
                        code_symbols.setdefault(m_name, self.ref_map[m_id][0])
        # 4. Type aliases
        for alias in ("Element", "Elements", "Decorator", "Component", "Components", "ComponentDecorator", "ElementDecorator"):
            if alias in self.symbol_map:
                code_symbols[alias] = self.symbol_map[alias]

        (vp_dir / "api_symbols.json").write_text(
            json.dumps({"symbols": code_symbols, "headers": self.header_map}, indent=2, sort_keys=True) + "\n",
            encoding="utf-8",
        )

        linked = self.autolink_markdown_guides()
        self.generate_llms_txt(dom_funcs, comp_funcs, class_entries)
        print(
            f"Generated {len(class_entries)} class pages, "
            f"{len(self.groups)} module pages, llms.txt/llms-full.txt, "
            f"and auto-linked {linked} references in guides."
        )

    def generate_llms_txt(
        self,
        dom_funcs: List[Tuple[str, str, str]],
        comp_funcs: List[Tuple[str, str, str]],
        class_entries: List[Tuple[str, str]],
    ) -> None:
        pub_dir = self.out_dir / "public"
        pub_dir.mkdir(parents=True, exist_ok=True)

        llms_lines = [
            "# FTXUI",
            "",
            "> C++ Functional Terminal User Interface library (v7.x). Zero dependencies, cross-platform (Linux, macOS, Windows, WebAssembly).",
            "",
            "## Important API Notes (v7+)",
            "- Use `ftxui::App` (from `<ftxui/component/app.hpp>`), NOT the deprecated `ScreenInteractive`.",
            "- Use `ftxui::Cell` / `ftxui::Surface` / `CellAt()`, NOT the deprecated `Pixel` / `Image` / `PixelAt()`.",
            "- Always pass a `Container` as the first argument to `Renderer(container, [&] { ... })` so events and focus reach child components.",
            "- Use `App::Post(...)` + `App::RequestAnimationFrame()` for thread-safe updates from background threads.",
            "",
            "## Guides",
            "- [Getting Started](/guide/getting-started)",
            "- [Mental Model](/guide/mental-model)",
            "- [Cookbook](/guide/cookbook)",
            "- [Cheat Sheet](/guide/cheatsheet)",
            "- [Troubleshooting & FAQ](/guide/troubleshooting)",
            "- [DOM Module](/guide/dom)",
            "- [Component Module](/guide/component)",
            "- [Screen Module](/guide/screen)",
            "- [Installation](/install/)",
            "",
            "## API Reference",
            "- [API Overview](/api/)",
            "- [Elements (ftxui/dom)](/api/dom): " + ", ".join(name for name, _, _ in dom_funcs),
            "- [Components (ftxui/component)](/api/component): " + ", ".join(name for name, _, _ in comp_funcs),
            "- [Screen (ftxui/screen)](/api/screen)",
            "- [Full LLM Reference](/llms-full.txt)",
            "",
            "## Key Classes",
        ]
        for slug, cname in class_entries:
            llms_lines.append(f"- [{cname}](/api/class/{slug})")
        (pub_dir / "llms.txt").write_text("\n".join(llms_lines) + "\n", encoding="utf-8")

        # Build llms-full.txt with core guides + concise API summaries
        full_parts = [
            "# FTXUI Complete Reference (llms-full.txt)",
            "",
            "NOTE: Use `ftxui::App` (not `ScreenInteractive`), `ftxui::Cell` (not `Pixel`), and `Renderer(container, ...)` for event routing.",
        ]
        for rel in (
            "guide/getting-started.md",
            "guide/mental-model.md",
            "guide/cookbook.md",
            "guide/cheatsheet.md",
            "guide/events-and-focus.md",
            "guide/custom-widgets.md",
            "guide/troubleshooting.md",
        ):
            p = self.out_dir / rel
            if p.exists():
                content = p.read_text(encoding="utf-8")
                # Strip Vue custom tags from plain text dump
                content = re.sub(r"<FtxuiDemo[^>]*/>\n?", "", content)
                full_parts.append("\n---\n\n" + content.strip())

        full_parts.append("\n---\n\n## Elements (`ftxui/dom`)\n\n" + ", ".join(f"`{n}`" for n, _, _ in dom_funcs))
        full_parts.append("\n## Components (`ftxui/component`)\n\n" + ", ".join(f"`{n}`" for n, _, _ in comp_funcs))
        (pub_dir / "llms-full.txt").write_text("\n\n".join(full_parts) + "\n", encoding="utf-8")


if __name__ == "__main__":
    xml_dir = Path(sys.argv[1]) if len(sys.argv) > 1 else Path("build/doc/doxygen/xml")
    out_dir = Path(sys.argv[2]) if len(sys.argv) > 2 else Path("docs_vitepress")
    DoxygenToVitePress(xml_dir, out_dir).run()
