#!/usr/bin/env python3
"""
Mermaid Diagram Validation Script

This script validates all Mermaid diagrams in markdown files within the repository.
It uses the mermaid-cli tool to render diagrams and check for syntax errors.
"""

import subprocess
import sys
import os
import glob
from pathlib import Path

def find_markdown_files(directory="."):
    """Find all markdown files in the repository."""
    markdown_files = []
    
    # Search for .md files in common documentation directories
    patterns = [
        "**/*.md",
        "docs/**/*.md",
        "**/README.md",
        "*.md"
    ]
    
    for pattern in patterns:
        markdown_files.extend(glob.glob(pattern, recursive=True))
    
    return list(set(markdown_files))  # Remove duplicates

def validate_mermaid_diagram(file_path):
    """Validate a single markdown file for Mermaid diagrams."""
    try:
        # Check if file contains mermaid diagrams
        with open(file_path, 'r', encoding='utf-8') as f:
            content = f.read()
            
        if '```mermaid' not in content:
            return True, "No Mermaid diagrams found"
        
        # Use mmdc to validate the diagram
        result = subprocess.run(
            ['mmdc', '--input', file_path, '--output', '/tmp/validation_test.svg'],
            capture_output=True,
            text=True,
            timeout=30
        )
        
        if result.returncode == 0:
            return True, f"Valid: {result.stdout.strip()}"
        else:
            error_msg = result.stderr.strip()
            if "Found" in result.stdout:
                # If diagrams were found but there were errors
                return False, f"Syntax error: {error_msg}"
            return False, f"Validation failed: {error_msg}"
    
    except FileNotFoundError:
        return False, "mermaid-cli (mmdc) not found. Install with: npm install -g @mermaid-js/mermaid-cli"
    except subprocess.TimeoutExpired:
        return False, "Validation timed out"
    except Exception as e:
        return False, f"Unexpected error: {str(e)}"

def main():
    """Main validation function."""
    print("🔍 Mermaid Diagram Validation")
    print("=" * 50)
    
    # Find all markdown files
    markdown_files = find_markdown_files()
    markdown_files = [f for f in markdown_files if os.path.isfile(f)]
    
    if not markdown_files:
        print("❌ No markdown files found in the repository.")
        return 1
    
    print(f"📚 Found {len(markdown_files)} markdown files to check")
    print()
    
    # Validate each file
    invalid_files = []
    valid_count = 0
    
    for file_path in markdown_files:
        relative_path = os.path.relpath(file_path)
        print(f"🔎 Checking: {relative_path}")
        
        is_valid, message = validate_mermaid_diagram(file_path)
        
        if is_valid:
            if "No Mermaid diagrams found" in message:
                print(f"  ⏭️  {message}")
            else:
                print(f"  ✅ {message}")
                valid_count += 1
        else:
            print(f"  ❌ {message}")
            invalid_files.append((file_path, message))
        
        print()
    
    # Summary
    print("=" * 50)
    print("📊 Validation Summary")
    print(f"  Files checked: {len(markdown_files)}")
    print(f"  Files with valid diagrams: {valid_count}")
    print(f"  Files with invalid diagrams: {len(invalid_files)}")
    
    if invalid_files:
        print()
        print("🔧 Files needing attention:")
        for file_path, error in invalid_files:
            relative_path = os.path.relpath(file_path)
            print(f"  • {relative_path}: {error}")
        return 1
    else:
        print()
        print("✅ All Mermaid diagrams are valid!")
        return 0

if __name__ == "__main__":
    sys.exit(main())