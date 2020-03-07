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

#include "PropertiesPanel.h"

#include <Corrade/Utility/Directory.h>
#include <Magnum/Magnum.h>
#include <Magnum/Math/ConfigurationValue.h>
#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>

#include "Themer.h"

using namespace Magnum;

PropertiesPanel::PropertiesPanel(FileNode* fileNode):
    _configuration{fileNode->path()}
{
    _fileNode = fileNode;
    _name = Utility::Directory::filename(_fileNode->path());
}

void PropertiesPanel::newFrame() {
    _isVisible = ImGui::Begin(_name.c_str(), &_isOpen);
    _isFocused = ImGui::IsWindowFocused();

    /* If the window is not visible, end the method here. */
    if(!_isVisible || !_isOpen) {
        ImGui::End();
        return;
    }

    Themer::setNextItemRightAlign("Name");
    std::string name = _configuration.value("name");
    if(ImGui::InputText("##Project.Name", &name))
        _configuration.setValue("name", name);

    Themer::setNextItemRightAlign("Window size");
    Vector2i windowSize = _configuration.value<Vector2i>("window_size");
    if(ImGui::DragInt2("##Project.WindowSize", windowSize.data()))
        _configuration.setValue<Vector2i>("window_size", windowSize);

    ImGui::End();
}