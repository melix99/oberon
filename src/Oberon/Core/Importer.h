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

#pragma once

#include "Light.h"
#include "Mesh.h"
#include "Script.h"
#include "Sprite.h"

#include <Corrade/Utility/ConfigurationGroup.h>
#include <Magnum/ResourceManager.h>
#include <Magnum/SceneGraph/TranslationRotationScalingTransformation3D.h>
#include <MagnumPlugins/PngImporter/PngImporter.h>

typedef SceneGraph::Object<SceneGraph::TranslationRotationScalingTransformation3D> Object3D;
typedef SceneGraph::Scene<SceneGraph::TranslationRotationScalingTransformation3D> Scene3D;

typedef ResourceManager<GL::AbstractShaderProgram, GL::Mesh, GL::Texture2D> OberonResourceManager;

class Importer {
    public:
        Importer(const std::string& projectPath): _projectPath{projectPath} {}

        Object3D* loadObject(Utility::ConfigurationGroup* objectConfig, Object3D* parent, OberonResourceManager& resourceManager, SceneGraph::DrawableGroup3D* drawables, ScriptGroup* scripts, LightGroup* lights);
        Object3D* loadChildrenObject(Utility::ConfigurationGroup* parentConfig, Object3D* parent, OberonResourceManager& resourceManager, SceneGraph::DrawableGroup3D* drawables, ScriptGroup* scripts, LightGroup* lights);

        void loadFeature(Utility::ConfigurationGroup* featureConfig, Object3D* object, OberonResourceManager& resourceManager, SceneGraph::DrawableGroup3D* drawables, ScriptGroup* scripts, LightGroup* lights);
        void resetObject(Object3D* object, Utility::ConfigurationGroup* objectConfig);

        void loadMeshFeature(Mesh& mesh, Utility::ConfigurationGroup* primitiveConfig, OberonResourceManager& resourceManager);

    private:
        std::string _projectPath;

        Trade::PngImporter _pngImporter;
};
