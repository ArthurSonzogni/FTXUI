#!/usr/bin/env python3
import sys
import re
import xml.etree.ElementTree as ET

def normalize_type_string(s):
    if not s:
        return ""
    # Standardize basic_string representations to std::string
    s = s.replace('std::__cxx11::basic_string<char,std::char_traits<char>,std::allocator<char>>', 'std::string')
    s = s.replace('std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char>>', 'std::string')
    s = s.replace('std::basic_string<char,std::char_traits<char>,std::allocator<char>>', 'std::string')
    s = s.replace('std::__cxx11::', 'std::')
    
    # Normalize spaces around brackets and commas
    s = re.sub(r'\s+', ' ', s)
    s = s.replace('< ', '<').replace(' >', '>').replace(' ,', ',')
    return s

def resolve_type_names(root):
    # Map from type-id to resolved stable name
    type_map = {}
    
    # First pass: gather all type elements with direct names
    todo = []
    for elem in root.iter():
        id_attr = elem.get('id')
        if id_attr:
            todo.append(elem)
            name = elem.get('name') or elem.get('mangled-name')
            if name:
                type_map[id_attr] = normalize_type_string(name)

    # Second pass: recursively resolve pointers, references, qualified types
    changed = True
    while changed:
        changed = False
        for elem in todo:
            id_attr = elem.get('id')
            if id_attr in type_map:
                continue
            
            target_id = elem.get('type-id')
            if target_id and target_id in type_map:
                target_name = type_map[target_id]
                tag = elem.tag
                if tag == 'pointer-type-def':
                    type_map[id_attr] = target_name + '*'
                    changed = True
                elif tag == 'reference-type-def':
                    type_map[id_attr] = target_name + '&'
                    changed = True
                elif tag == 'qualified-type-def':
                    const_str = 'const ' if elem.get('const') == 'yes' else ''
                    type_map[id_attr] = const_str + target_name
                    changed = True
                else:
                    type_map[id_attr] = target_name
                    changed = True
                    
    return type_map

def serialize_element(elem, type_map):
    # Resolve type-id to stable type name if present
    type_id = elem.get('type-id')
    resolved_type = ""
    if type_id and type_id in type_map:
        resolved_type = type_map[type_id]

    # Gather all attributes except transient ones
    attrs = {}
    exclude_attrs = {'id', 'type-id', 'naming-typedef-id', 'elf-symbol-id', 'interface-type', 'filepath', 'line', 'column'}
    for k, v in elem.attrib.items():
        if k not in exclude_attrs:
            attrs[k] = normalize_type_string(v)
            
    if resolved_type:
        attrs['type'] = resolved_type

    # Serialize attributes sorted by key
    attr_parts = []
    for k, v in sorted(attrs.items()):
        attr_parts.append(f"{k}='{v}'")
    attr_str = " ".join(attr_parts)
    if attr_str:
        attr_str = " " + attr_str

    # Recursively serialize children and sort them
    child_strings = []
    for child in elem:
        child_str = serialize_element(child, type_map)
        if child_str:
            child_strings.append(child_str)
            
    child_strings.sort()
    
    if child_strings:
        children_str = "".join(child_strings)
        return f"<{elem.tag}{attr_str}>{children_str}</{elem.tag}>"
    else:
        return f"<{elem.tag}{attr_str}/>"

def clean_and_normalize():
    try:
        tree = ET.parse(sys.stdin)
        root = tree.getroot()
    except Exception as e:
        sys.stderr.write(f"Error parsing XML: {e}\n")
        sys.exit(1)

    type_map = resolve_type_names(root)
    normalized_lines = []

    # 1. Extract and serialize namespace elements
    for ns in root.findall(".//namespace-decl[@name='ftxui']"):
        ns_str = serialize_element(ns, type_map)
        if ns_str:
            normalized_lines.append(ns_str)

    # 2. Extract and serialize elf-symbols belonging to ftxui
    for sym in root.findall(".//elf-symbol"):
        name = sym.get('name')
        if name and ('ftxui' in name or '_ZN5ftxui' in name or '_ZNK5ftxui' in name):
            sym_str = serialize_element(sym, type_map)
            if sym_str:
                normalized_lines.append(sym_str)

    # Sort lines to be completely independent of compiler parsing order
    for line in sorted(list(set(normalized_lines))):
        print(line)

if __name__ == "__main__":
    clean_and_normalize()
