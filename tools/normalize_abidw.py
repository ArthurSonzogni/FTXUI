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

def clean_and_normalize():
    try:
        tree = ET.parse(sys.stdin)
        root = tree.getroot()
    except Exception as e:
        sys.stderr.write(f"Error parsing XML: {e}\n")
        sys.exit(1)

    type_map = resolve_type_names(root)
    normalized_lines = []

    # We extract two things:
    # 1. All elements defined inside <namespace-decl name='ftxui'> (nested or direct)
    # 2. All <elf-symbol> tags that belong to the ftxui namespace
    
    # 1. Extract namespace elements
    for ns in root.findall(".//namespace-decl[@name='ftxui']"):
        for elem in ns.iter():
            if elem.tag in ['namespace-decl', 'abi-instr']:
                continue
            
            # Create a simplified string representation of the element
            attrs = []
            # Resolve type-id to stable type name
            type_id = elem.get('type-id')
            if type_id and type_id in type_map:
                attrs.append(f"type='{type_map[type_id]}'")
                
            # Add other stable attributes
            for attr in ['name', 'mangled-name', 'size-in-bits', 'layout-offset-in-bits', 'access', 'visibility', 'binding']:
                val = elem.get(attr)
                if val:
                    val = normalize_type_string(val)
                    attrs.append(f"{attr}='{val}'")
                    
            attr_str = " ".join(sorted(attrs))
            normalized_lines.append(f"<{elem.tag} {attr_str}/>")

    # 2. Extract elf-symbols belonging to ftxui
    for sym in root.findall(".//elf-symbol"):
        name = sym.get('name')
        if name and ('ftxui' in name or '_ZN5ftxui' in name or '_ZNK5ftxui' in name):
            attrs = []
            for attr in ['name', 'type', 'binding', 'visibility', 'is-defined']:
                val = sym.get(attr)
                if val:
                    attrs.append(f"{attr}='{val}'")
            attr_str = " ".join(sorted(attrs))
            normalized_lines.append(f"<elf-symbol {attr_str}/>")

    # Sort lines to be completely independent of compiler parsing order
    for line in sorted(list(set(normalized_lines))):
        print(line)

if __name__ == "__main__":
    clean_and_normalize()
