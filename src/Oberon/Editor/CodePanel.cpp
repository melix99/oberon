/*
    This file is part of Oberon.

    Copyright (c) 2019-2020 Marco Melorio

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.
*/

#include "CodePanel.h"

#include <Corrade/Utility/Directory.h>

CodePanel::CodePanel(FileNode* fileNode) {
    _fileNode = fileNode;
    _name = Utility::Directory::filename(_fileNode->path());

    std::string codeText = Utility::Directory::readString(_fileNode->path());
    Containers::arrayAppend(_lines, Line{});

    for(char& chr : codeText) {
        if(chr == '\n')
            Containers::arrayAppend(_lines, Line{});
        else
            Containers::arrayAppend(_lines.back(), Glyph{chr, PaletteIndex::Default});
    }
}

void CodePanel::newFrame() {
    _isVisible = ImGui::Begin(_name.c_str(), &_isOpen);

    _isFocused = ImGui::IsWindowFocused();

    /* If the window is not visible, end the method here. */
    if(!_isVisible || !_isOpen) {
        ImGui::End();
        return;
    }

    const Float textSpacing = 15.0f;

    Float maxLineNumWidth = ImGui::CalcTextSize(std::to_string(_lines.size()).c_str()).x;
    ImVec2 lineStartPos = ImGui::GetCursorScreenPos();
    ImDrawList* drawList = ImGui::GetWindowDrawList();

    ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]);
    for(size_t lineIndex = 0; lineIndex < _lines.size(); ++lineIndex) {
        /* Draw line number */
        _textBuffer = std::to_string(lineIndex + 1);
        Float lineNumWidth = ImGui::CalcTextSize(_textBuffer.c_str()).x;
        drawList->AddText(ImVec2(lineStartPos.x + maxLineNumWidth - lineNumWidth, lineStartPos.y), getColorFromIndex(PaletteIndex::LineNumber), _textBuffer.c_str());

        /* Draw text */
        ImVec2 textStartPos(lineStartPos.x + maxLineNumWidth + textSpacing, lineStartPos.y);
        Line& line = _lines[lineIndex];
        Float glyphOffset = 0.0f;
        for(size_t i = 0; i < line.size(); ++i) {
            _textBuffer = std::string(1, line[i]._character);
            drawList->AddText(ImVec2(textStartPos.x + glyphOffset, textStartPos.y), getColorFromIndex(PaletteIndex::Default), _textBuffer.c_str());

            Float glyphWidth = ImGui::CalcTextSize(_textBuffer.c_str()).x;
            glyphOffset += glyphWidth;
        }

        lineStartPos.y += ImGui::GetTextLineHeightWithSpacing();
    }
    ImGui::PopFont();

    ImGui::End();
}

ImU32 CodePanel::getColorFromIndex(PaletteIndex index) {
    constexpr ImU32 ColorMap[] {
        0xffe6e6e6,   /* Default */
        0xff999999    /* LineNumber */
    };

    return ColorMap[UnsignedInt(index)];
}