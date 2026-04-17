#!/usr/bin/env python3
"""
doc-check skill implementation
Analyzes documentation files for correct persona placement
"""

import os
import re
import subprocess
from pathlib import Path
from typing import List, Dict, Tuple

# Persona definitions
PERSONA_PATHS = {
    "builder": "docs/builders/",
    "musician": "docs/musicians/", 
    "developer": "docs/developers/"
}

PERSONA_KEYWORDS = {
    "builder": [
        "solder", "wiring", "GPIO", "breadboard", "schematic", "enclosure",
        "hardware", "build", "assemble", "circuit", "resistor", "LED",
        "button wiring", "pin assignment", "voltage", "current", "ohm",
        "multimeter", "screwdriver", "drill", "3D print", "enclosure"
    ],
    "musician": [
        "press button", "profile", "Bluetooth", "footswitch", "pedal",
        "play", "perform", "use", "operate", "select profile", "change profile",
        "connect", "pair", "battery", "power on", "volume", "effect",
        "sound", "audio", "MIDI", "sustain", "expression", "bank",
        "patch", "preset", "amp", "guitar", "bass", "keyboard"
    ],
    "developer": [
        "CMake", "PR", "architecture", "clang", "API", "contributing",
        "code", "program", "compile", "build system", "pull request",
        "merge", "branch", "repository", "version control", "debug",
        "test", "unit test", "integration", "continuous integration",
        "CI/CD", "pipeline", "docker", "container", "virtual machine",
        "IDE", "editor", "linter", "formatter", "static analysis",
        "coverage", "mock", "stub", "refactor", "design pattern"
    ]
}

class DocCheck:
    def __init__(self):
        self.files_to_check = []
        self.results = []
        
    def get_changed_files(self) -> List[str]:
        """Get list of changed .md files relative to main branch"""
        try:
            result = subprocess.run(
                ["git", "diff", "--name-only", "main...HEAD", "--", "*.md"],
                capture_output=True,
                text=True,
                check=True
            )
            return [f.strip() for f in result.stdout.splitlines() if f.strip()]
        except subprocess.CalledProcessError:
            return []
    
    def filter_files(self, files: List[str]) -> List[str]:
        """Filter out internal task files"""
        return [f for f in files if not f.startswith("docs/developers/tasks/")]
    
    def get_path_based_persona(self, filepath: str) -> str:
        """Determine persona from file path"""
        for persona, path_prefix in PERSONA_PATHS.items():
            if filepath.startswith(path_prefix):
                return persona
        return "unknown"
    
    def analyze_content(self, filepath: str, content: str) -> Tuple[str, int]:
        """Analyze file content and return suggested persona with confidence score"""
        keyword_counts = {persona: 0 for persona in PERSONA_KEYWORDS}
        
        # Count keyword matches (case-insensitive)
        content_lower = content.lower()
        for persona, keywords in PERSONA_KEYWORDS.items():
            for keyword in keywords:
                if keyword.lower() in content_lower:
                    keyword_counts[persona] += 1
        
        # Determine suggested persona based on highest keyword count
        suggested_persona = max(keyword_counts, key=keyword_counts.get)
        confidence_score = keyword_counts[suggested_persona]
        
        return suggested_persona, confidence_score
    
    def calculate_confidence_level(self, path_persona: str, suggested_persona: str, score: int) -> str:
        """Calculate confidence level for the classification"""
        if path_persona == suggested_persona:
            if score >= 3:
                return "High"
            elif score >= 1:
                return "Medium"
            else:
                return "Low"
        else:
            return "Mismatch"
    
    def get_keyword_examples(self, content: str, persona: str, limit: int = 3) -> List[str]:
        """Get example keywords found in content"""
        content_lower = content.lower()
        found_keywords = []
        
        for keyword in PERSONA_KEYWORDS[persona]:
            if keyword.lower() in content_lower:
                found_keywords.append(keyword)
                if len(found_keywords) >= limit:
                    break
        
        return found_keywords
    
    def check_file(self, filepath: str) -> Dict:
        """Check a single file for persona placement"""
        try:
            # Read file content
            with open(filepath, 'r', encoding='utf-8') as f:
                content = f.read()
            
            # Determine personas
            path_persona = self.get_path_based_persona(filepath)
            suggested_persona, score = self.analyze_content(filepath, content)
            
            # Calculate confidence
            confidence = self.calculate_confidence_level(path_persona, suggested_persona, score)
            
            # Get keyword examples for reasoning
            if confidence in ["High", "Medium", "Mismatch"]:
                examples = self.get_keyword_examples(content, suggested_persona)
                reason = f"Contains {suggested_persona} keywords: {', '.join(examples)}"
            else:
                reason = "No clear persona keywords found"
            
            return {
                "file": filepath,
                "current_persona": path_persona,
                "suggested_persona": suggested_persona,
                "confidence": confidence,
                "reason": reason
            }
        
        except Exception as e:
            return {
                "file": filepath,
                "current_persona": "unknown",
                "suggested_persona": "unknown",
                "confidence": "Error",
                "reason": f"Error reading file: {str(e)}"
            }
    
    def run_check(self) -> str:
        """Run the documentation check"""
        # Get changed files
        changed_files = self.get_changed_files()
        
        if not changed_files:
            return "No documentation changes to check."
        
        # Filter files
        files_to_check = self.filter_files(changed_files)
        
        if not files_to_check:
            return "No documentation changes to check (only internal task files changed)."
        
        # Check each file
        results = []
        for filepath in files_to_check:
            result = self.check_file(filepath)
            results.append(result)
        
        # Generate report
        report_lines = [
            "Documentation Persona Check",
            "============================",
            ""
        ]
        
        correctly_placed = 0
        needs_review = 0
        
        for result in results:
            report_lines.append(f"File: {result['file']}")
            report_lines.append(f"Current persona: {result['current_persona']}")
            report_lines.append(f"Suggested persona: {result['suggested_persona']}")
            report_lines.append(f"Confidence: {result['confidence']}")
            report_lines.append(f"Reason: {result['reason']}")
            report_lines.append("")
            
            if result['confidence'] in ["High", "Medium"] and result['current_persona'] == result['suggested_persona']:
                correctly_placed += 1
            else:
                needs_review += 1
        
        # Add summary
        report_lines.append(f"Summary:")
        report_lines.append(f"- Total files checked: {len(results)}")
        report_lines.append(f"- Correctly placed: {correctly_placed}")
        report_lines.append(f"- Needs review: {needs_review}")
        
        return "\n".join(report_lines)

if __name__ == "__main__":
    checker = DocCheck()
    print(checker.run_check())