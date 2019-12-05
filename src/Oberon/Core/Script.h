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

#include <string>

#include <Magnum/SceneGraph/AbstractGroupedFeature.h>
#include <pybind11/pybind11.h>

using namespace Magnum;
namespace py = pybind11;

class Script;
typedef SceneGraph::FeatureGroup3D<Script> ScriptGroup;

class Script: public SceneGraph::AbstractGroupedFeature3D<Script> {
    public:
        explicit Script(SceneGraph::AbstractObject3D& object, ScriptGroup* scripts, const std::string& scriptPath): SceneGraph::AbstractGroupedFeature3D<Script>{object, scripts}, _scriptPath(scriptPath) {}

        std::string scriptPath() const { return _scriptPath; }

        Script& setScriptPath(const std::string& scriptPath) {
            _scriptPath = scriptPath;
            return *this;
        }

        py::module& pyModule() { return _pyModule; }

    private:
        std::string _scriptPath;
        py::module _pyModule;
};