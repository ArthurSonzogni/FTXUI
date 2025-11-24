#!/usr/bin/env python3

import os
import subprocess
import shutil
import tempfile
import json
from pathlib import Path
from typing import List, Dict

# --- Configuration ---
# URL for the translations repository. This is where other language branches reside.
TRANSLATIONS_REPO_URL = "https://github.com/ArthurSonzogni/ftxui-translations"
# --- End Configuration ---

# Mapping of language codes to their display names for the dropdown menu.
# Key: Standard BCP 47/ISO 639-1 Code
# Value: [Native Name, Doxygen Name]
LANG_NAME_MAP = {
    "af": ["Afrikaans", "Afrikaans"],
    "ar": ["العربية", "Arabic"],
    "bg": ["Български", "Bulgarian"],
    "ca": ["Català", "Catalan"],
    "cs": ["Čeština", "Czech"],
    "da": ["Dansk", "Danish"],
    "de": ["Deutsch", "German"],
    "el": ["Ελληνικά", "Greek"],
    "en": ["English", "English"],
    "eo": ["Esperanto", "Esperanto"],
    "es": ["Español", "Spanish"],
    "fi": ["Suomi", "Finnish"],
    "fr": ["Français", "French"],
    "hi": ["हिन्दी", "Hindi"],
    "hr": ["Hrvatski", "Croatian"],
    "hu": ["Magyar", "Hungarian"],
    "hy": ["Հայերեն", "Armenian"],
    "id": ["Bahasa Indonesia", "Indonesian"],
    "it": ["Italiano", "Italian"],
    "ja": ["日本語", "Japanese-en"],
    "ko": ["한국어", "Korean-en"],
    "lt": ["Lietuvių", "Lithuanian"],
    "lv": ["Latviešu", "Latvian"],
    "mk": ["Македонски", "Macedonian"],
    "nl": ["Nederlands", "Dutch"],
    "no": ["Norsk", "Norwegian"],
    "pl": ["Polski", "Polish"],
    "pt": ["Português", "Portuguese"],
    "ro": ["Română", "Romanian"],
    "ru": ["Русский", "Russian"],
    "sk": ["Slovenčina", "Slovak"],
    "sl": ["Slovenščina", "Slovene"],
    "sr": ["Српски", "Serbian"],
    "sv": ["Svenska", "Swedish"],
    "tr": ["Türkçe", "Turkish"],
    "uk": ["Українська", "Ukrainian"],
    "vi": ["Tiếng Việt", "Vietnamese"],
    "zh-CH": ["中文 (简体)", "Chinese"],
    "zh-TW": ["中文 (繁體)", "Chinese-Traditional"],
}


class DocInfo:
    """A structure to hold all information about a single documentation build."""
    def __init__(self, lang: str, version_name: str, is_main_version: bool, output_root: Path, is_primary_lang: bool = True):
        self.lang = lang
        self.version_name = version_name  # e.g., "main", "v6.1.9"
        self.is_main_version = is_main_version
        self.is_primary_lang = is_primary_lang
        
        if self.is_primary_lang:
            if self.is_main_version:
                # English Main: Deployed to the root directory
                self.dest_dir = output_root
            else:
                # English Versions: Deployed to /en/<version_name>
                self.dest_dir = output_root / "en" / version_name
        else:
            # Translated Docs (only 'main' version for now): Deployed to /<lang_code>/
            self.dest_dir = output_root / lang

        # The path to this version's index.html, relative to the output root.
        self.index_path_from_root = self.dest_dir / "index.html"

    @property
    def key(self) -> str:
        """A unique key for this documentation set (e.g., 'en-main', 'fr-main', 'en-v6.1.9')."""
        return f"{self.lang}-{self.version_name}"

    def __repr__(self) -> str:
        return f"DocInfo(lang='{self.lang}', version='{self.version_name}', dest_dir='{self.dest_dir}')"

def run_command(command: List[str], check: bool = True, cwd: Path = None):
    """
    Runs a command, prints its output, and handles errors.
    """
    command_str = ' '.join(command)
    print(f"Executing: {command_str} in {cwd or Path.cwd()}")
    try:
        result = subprocess.run(
            command,
            capture_output=True,
            text=True,
            check=check,
            cwd=cwd
        )
        if result.stdout:
            print(result.stdout)
        if result.stderr:
            print(result.stderr)
        return result
    except subprocess.CalledProcessError as e:
        print(f"ERROR: Command failed with exit code {e.returncode}")
        print(f"Command: {command_str}")
        if e.stdout:
            print("--- STDOUT ---")
            print(e.stdout)
        if e.stderr:
            print("--- STDERR ---")
            print(e.stderr)
        raise  # Re-raise the exception to halt the script

def get_switchers_js(
    current_doc: DocInfo,
    all_docs: List[DocInfo],
    current_html_file: Path
) -> str:
    """
    Generates the JavaScript for both the version and language switcher dropdowns.
    """
    
    # 1. Prepare Language Data (Links to the main version/entry point of each language)
    language_map: Dict[str, str] = {}
    language_display_names: Dict[str, str] = {}
    language_doxygen_names: Dict[str, str] = {}
    
    # Filter for the main entry point of each language (e.g., /fr/, /en/ and /)
    main_language_docs = {}
    for doc in all_docs:
        # Use the main version for its language as the entry point
        if doc.is_main_version or (doc.is_primary_lang and doc.version_name == 'main'):
            if doc.lang not in main_language_docs:
                main_language_docs[doc.lang] = doc

    # Calculate relative paths for the language switcher
    for lang_code, doc in main_language_docs.items():
        relative_path = os.path.relpath(doc.index_path_from_root, current_html_file.parent)
        language_map[lang_code] = relative_path
        # Use the map for display name, fall back to code if not found
        language_display_names[lang_code] = LANG_NAME_MAP.get(lang_code,
                                                              lang_code)[0]
        language_doxygen_names[lang_code] = LANG_NAME_MAP.get(lang_code,
                                                              lang_code)[1]
        
    language_names = list(language_map.keys())
    # Sort languages: 'en' first, then others alphabetically.
    language_names.sort(key=lambda x: (x != 'en', x))


    # 2. Prepare Version Data (Only versions for the current language)
    version_docs = [doc for doc in all_docs if doc.lang == current_doc.lang]
    version_names = [v.version_name for v in version_docs]
    version_paths: Dict[str, str] = {}
    
    # Calculate relative paths for the version switcher
    for version_doc in version_docs:
        relative_path = os.path.relpath(version_doc.index_path_from_root, current_html_file.parent)
        version_paths[version_doc.version_name] = relative_path

    # Use json.dumps for safe serialization of data into JavaScript.
    langs_json = json.dumps(language_names)
    lang_paths_json = json.dumps(language_map)
    lang_display_json = json.dumps(language_display_names)
    versions_json = json.dumps(version_names)
    version_paths_json = json.dumps(version_paths)
    current_lang_json = json.dumps(current_doc.lang)
    current_version_json = json.dumps(current_doc.version_name)

    # Note: We are using Doxygen's #projectnumber container to inject both switchers.
    # We will wrap the original element with a new container.
    return f"""
document.addEventListener('DOMContentLoaded', function() {{
  const projectNumber = document.getElementById('projectname');

  if (!projectNumber) {{
    console.warn('Doxygen element with ID "projectnumber" not found. Cannot add version switcher.');
    return;
  }}

  const langs = {langs_json};
  const lang_paths = {lang_paths_json};
  const lang_display = {lang_display_json};
  const versions = {versions_json};
  const version_paths = {version_paths_json};
  const currentLang = {current_lang_json};
  const currentVersion = {current_version_json};

  // Helper function to create a styled select element
  const createSelect = (options, current, paths, label, displayMap = null) => {{
    const select = document.createElement('select');
    select.title = label;
    select.onchange = function() {{
      const selectedValue = this.value;
      if (selectedValue in paths) {{
        window.location.href = paths[selectedValue];
      }}
    }};

    // Sort versions: 'main' first, then others numerically descending.
    options.sort((a, b) => {{
      if (a === 'main') return -1;
      if (b === 'main') return 1;
      return b.localeCompare(a, undefined, {{ numeric: true, sensitivity: 'base' }});
    }});

    options.forEach(v => {{
      const option = document.createElement('option');
      option.value = v;
      // Use the displayMap if provided, otherwise default to the value (v)
      option.textContent = displayMap ? displayMap[v] : v; 
      if (v === current) {{
        option.selected = true;
      }}
      select.appendChild(option);
    }});

    // Apply some styling to make it look good.
    Object.assign(select.style, {{
      backgroundColor: 'rgba(0, 0, 0, 0.8)',
      color: 'white',
      border: '1px solid rgba(255, 255, 255, 0.2)',
      padding: '5px',
      borderRadius: '5px',
      fontSize: '14px',
      fontFamily: 'inherit',
      margin: '0 5px 0 0',
      cursor: 'pointer'
    }});
    return select;
  }};

  // 1. Create Language Switcher, passing the language display names map
  const langSelect = createSelect(langs, currentLang, lang_paths, 'Select Language', lang_display);
  
  // 2. Create Version Switcher
  const versionSelect = createSelect(versions, currentVersion, version_paths, 'Select Version');

  // 3. Create FTXUI title.
  const ftxuiTitle = document.createElement('span');
  ftxuiTitle.textContent = 'FTXUI: ';
  Object.assign(ftxuiTitle.style, {{
    color: 'white',
    fontSize: '20px',
    fontWeight: 'bold',
    marginRight: '10px'
  }});

  // 3. Create a container to hold both selectors
  const container = document.createElement('div');
  container.id = 'version-lang-switchers';
  Object.assign(container.style, {{
    display: 'flex',
    alignItems: 'center',
    justifyContent: 'flex-end',
    width: 'auto'
  }});
  container.appendChild(ftxuiTitle);
  container.appendChild(langSelect);
  container.appendChild(versionSelect);

  Object.assign(container.style, {{
    backgroundColor: 'rgba(0, 0, 0, 0.5)',
    padding: '5px 10px',
    borderRadius: '8px'
  }});


  // Replace the Doxygen project number element with our container.
  projectNumber.replaceWith(container);

  // Clean up the original Doxygen project number text if it still exists nearby
  const parent = container.parentElement;
  if (parent) {{
    const textNode = Array.from(parent.childNodes).find(n => n.nodeType === 3 && n.textContent.trim() !== '');
    if (textNode) {{
      textNode.remove();
    }}
  }}
}});
"""

def build_doc_from_git(doc_info: DocInfo, build_root: Path, repo_url: str, branch_or_tag: str, temp_repo_dir: Path):
    """
    Handles checking out the source from a git reference, running the build,
    and copying the output to the final destination.
    
    :param doc_info: The DocInfo object for the build.
    :param build_root: The temporary root directory for all builds.
    :param repo_url: The URL or path to the git repository.
    :param branch_or_tag: The git reference (branch or tag) to check out.
    :param temp_repo_dir: The path to the temporary cloned repository directory.
    """
    print(f"--- Building {doc_info.key} (Source: {branch_or_tag} from {repo_url}) ---")
    
    # 1. Archive the source code from the repository reference.
    version_src_dir = build_root / f"src_{doc_info.key}"
    version_src_dir.mkdir(parents=True, exist_ok=True)

    archive_path = version_src_dir / "source.tar"
    
    # Determine the directory to run git archive from (it needs to be the root of the git repo)
    git_run_dir = temp_repo_dir if temp_repo_dir.is_dir() else Path.cwd()

    run_command([
        "git", "archive", branch_or_tag,
        "--format=tar", f"--output={archive_path}"
    ], cwd=git_run_dir)
    
    run_command(["tar", "-xf", str(archive_path)], cwd=version_src_dir)
    archive_path.unlink()

    # 2. Configure and build the docs using CMake.
    version_build_dir = build_root / f"build_{doc_info.key}"
    version_build_dir.mkdir()

    # 2.a Update doc/Doxyfile.in to set the correct language.
    doxyfile_in_path = version_src_dir / "doc" / "Doxyfile.in"
    if not doxyfile_in_path.is_file():
        print(f"FATAL: Doxyfile.in not found at {doxyfile_in_path} for {doc_info.key}")
        exit(1)
    doxyfile_content = doxyfile_in_path.read_text(encoding='utf-8')
    # Replace the keyword "English" with the appropriate Doxygen language name.
    lang_doxygen_name = LANG_NAME_MAP.get(doc_info.lang, [doc_info.lang,
                                                          doc_info.lang])[1]
    doxyfile_content = doxyfile_content.replace("English", lang_doxygen_name)

    
    # Assuming CMakeLists.txt is in the root of the extracted source
    run_command([ 
        "cmake", str(version_src_dir), 
        "-DFTXUI_BUILD_DOCS=ON", 
        '-DFTXUI_BUILD_EXAMPLES=ON' # Required for the Doxygen build target
    ], cwd=version_build_dir)
    
    run_command(["make", "doc"], cwd=version_build_dir)

    # 3. Copy the generated HTML files to the final destination.
    doxygen_html_dir = version_build_dir / "doc" / "doxygen" / "html"
    if not doxygen_html_dir.is_dir():
        print(f"FATAL: Doxygen HTML output not found for {doc_info.key} at {doxygen_html_dir}")
        exit(1)

    print(f"Copying files to: {doc_info.dest_dir}")
    shutil.copytree(doxygen_html_dir, doc_info.dest_dir, dirs_exist_ok=True)

def main():
    """Main function to build multi-version and multi-language documentation."""
    root_dir = Path.cwd()
    output_dir = root_dir / "multiversion_docs"

    print("--- 1. Cleaning up old documentation ---")
    if output_dir.exists():
        shutil.rmtree(output_dir)
    output_dir.mkdir(parents=True, exist_ok=True)

    all_docs: List[DocInfo] = []

    # --- 2. Gather English (Primary) Versions from the current repository ---
    print("--- 2a. Getting English versions (main repo) ---")
    
    # Get all tags that start with 'v' for versioning
    git_tags_result = run_command(["git", "tag", "--list", "v*"])
    
    # Start with 'main', then add sorted tags (reverse numerical order)
    english_versions = ["main"] + sorted(
        git_tags_result.stdout.splitlines(),
        reverse=True
    )
    
    print(f"English versions to build: {', '.join(english_versions)}")

    for name in english_versions:
        is_main = name == "main"
        all_docs.append(DocInfo("en", name, is_main, output_dir, is_primary_lang=True))

    with tempfile.TemporaryDirectory() as build_dir_str:
        build_dir = Path(build_dir_str)
        temp_translations_repo = build_dir / "translations_repo"

        # --- 2b. Gather Translated Language Branches ---
        print("\n--- 2b. Cloning translations repo and getting language branches (excluding 'main') ---")
        try:
            # Clone the translations repository
            run_command(["git", "clone", TRANSLATIONS_REPO_URL, str(temp_translations_repo)])

            # Get remote branches (e.g., origin/fr, origin/zh-CH) and map them to lang codes
            translation_branches_result = run_command(
                ["git", "branch", "-r", "--list", "origin/*"], 
                cwd=temp_translations_repo
            )
            
            # Filter and map to language codes
            language_branches = []
            for line in translation_branches_result.stdout.splitlines():
                branch_name = line.strip()
                
                # Ignore lines that are references (like 'origin/HEAD -> origin/main')
                if '->' in branch_name:
                    continue
                
                # Extract the language code (e.g., 'fr' from 'origin/fr')
                if branch_name.startswith('origin/'):
                    # The name after 'origin/'
                    lang_code = branch_name.split('origin/')[1] 
                    
                    # Explicitly exclude 'main' as requested by the user
                    if lang_code != 'main': 
                        language_branches.append(lang_code)

            print(f"Translation languages to build: {', '.join(language_branches)}")
            
            for lang in language_branches:
                # For translations, we treat them as the 'main' version for that language
                # The branch name is the language code (e.g., 'fr' branch)
                all_docs.append(DocInfo(lang, "main", True, output_dir, is_primary_lang=False))

        except Exception as e:
            print(f"Warning: Could not clone or process translations repository: {e}")
            # Continue with English docs if translation cloning fails

        # --- 3. Build documentation for each DocInfo ---
        for doc in all_docs:
            if doc.is_primary_lang:
                # English docs are archived from the current directory (root_dir)
                build_doc_from_git(doc, build_dir, "origin", doc.version_name, root_dir)
            else:
                # Translated docs are archived from the cloned translation repo
                # FIX: Use 'origin/<lang>' as the git reference because 'git archive' 
                # inside the repository only knows remote-tracking branches.
                translation_git_ref = f"origin/{doc.lang}"
                build_doc_from_git(doc, build_dir, TRANSLATIONS_REPO_URL, translation_git_ref, temp_translations_repo)

        # --- 4. Inject version and language switchers into all HTML files ---
        print("\n--- Injecting version and language switcher JavaScript ---")
        
        # A set to keep track of processed directories
        processed_dirs = set() 
        
        for doc in all_docs:
            if not doc.dest_dir.exists() or doc.dest_dir in processed_dirs:
                continue

            print(f"Processing HTML files in: {doc.dest_dir}")

            # For the main 'en' directory, we need to handle the structure carefully 
            # as it contains the root, but we only want to inject once per file.
            html_files: List[Path] = []
            # We need all language branch names for exclusion, including 'en'.
            all_lang_dirs = [d.lang for d in all_docs if not d.is_primary_lang] + ['en']

            if doc.is_main_version and doc.is_primary_lang:
                # This is the root level ('/')
                # Find all HTML files, but explicitly exclude the language subdirectories ('en', 'fr', etc.)
                html_files.extend(doc.dest_dir.glob("*.html"))
                for subdir in doc.dest_dir.iterdir():
                    if subdir.is_dir() and subdir.name not in all_lang_dirs:
                        html_files.extend(subdir.rglob("*.html"))
            else:
                # This handles /en/vX.Y.Z/ or /fr/ 
                html_files = list(doc.dest_dir.rglob("*.html"))
            
            # Since multiple DocInfos might point to the same file (e.g., index.html), 
            # we inject the script relative to that specific file's context.
            for html_file in html_files:
                js_script = get_switchers_js(doc, all_docs, html_file)
                script_tag = f'<script>{js_script}</script>'

                content = html_file.read_text(encoding='utf-8')
                # Inject the script right before the closing body tag.
                new_content = content.replace("</body>", f"{script_tag}\n</body>")
                html_file.write_text(new_content, encoding='utf-8')

            processed_dirs.add(doc.dest_dir)

    print("\n--- 5. Finalizing ---")
    print("Multi-version and multi-language documentation generated successfully!")
    print(f"Output located in: {output_dir.resolve()}")

if __name__ == "__main__":
    main()
