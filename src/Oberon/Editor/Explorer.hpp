/*
    MIT License

    Copyright (c) 2019 Marco Melorio

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

#pragma once

#include <Corrade/Utility/Directory.h>
#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>
#include <vector>

using namespace Corrade;

struct FileNode {
    typedef std::unique_ptr<FileNode> Ptr;

    FileNode(const std::string& path, FileNode* parent = nullptr);
    FileNode* addChild(const std::string& path = "");

    std::string path;

    FileNode* parent;
    std::vector<Ptr> children;
    bool is_selected;
};

class Explorer {
public:
    Explorer(const std::string& project_path);
    void newFrame();

    FileNode* clicked_node;

private:
    void updateFileNodeChildren(FileNode* node);
    void displayFileTree(FileNode* node);
    void removeEntireFile(const std::string& path);

    static bool sortFileNodes(const FileNode::Ptr& a, const FileNode::Ptr& b);

    FileNode root_node;
    std::vector<FileNode*> selected_nodes;
    bool delete_selected_nodes;
};
