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

#include "CollectionPanel.h"

#include <algorithm>
#include <Corrade/Utility/Directory.h>
#include <Magnum/GL/Mesh.h>
#include <Magnum/GL/RenderbufferFormat.h>
#include <Magnum/GL/TextureFormat.h>
#include <Magnum/ImGuiIntegration/Integration.h>
#include <Magnum/MeshTools/Compile.h>
#include <Magnum/Primitives/Grid.h>
#include <Magnum/Trade/MeshData.h>
#include <Oberon/Core/Importer.h>
#include <Oberon/Core/Light.h>
#include <Oberon/Core/Mesh.h>
#include <Oberon/Core/Script.h>
#include <Oberon/Core/ScriptManager.h>

#include "FileNode.h"
#include "ObjectNode.h"

CollectionPanel::CollectionPanel(FileNode* fileNode, OberonResourceManager& resourceManager, Importer& importer, ScriptManager& scriptManager, const Vector2i& viewportTextureSize, const Vector2& dpiScaleRatio, const std::string& projectPath):
    AbstractPanel{fileNode}, _collectionConfig{fileNode->path()}, _resourceManager{resourceManager}, _importer{importer}, _scriptManager{scriptManager}, _viewportTextureSize{viewportTextureSize}, _dpiScaleRatio{dpiScaleRatio}, _projectPath(projectPath)
{
    _name = Utility::Directory::filename(_fileNode->path());

    _viewportTexture.setStorage(1, GL::TextureFormat::RGBA8, _viewportTextureSize*_dpiScaleRatio);
    _depth.setStorage(GL::RenderbufferFormat::Depth24Stencil8, _viewportTextureSize*_dpiScaleRatio);
    _objectId.setStorage(GL::RenderbufferFormat::R32UI, _viewportTextureSize*_dpiScaleRatio);

    _framebuffer = GL::Framebuffer{{}};
    _framebuffer.attachTexture(GL::Framebuffer::ColorAttachment{0}, _viewportTexture, 0)
        .attachRenderbuffer(GL::Framebuffer::ColorAttachment{1}, _objectId)
        .attachRenderbuffer(GL::Framebuffer::BufferAttachment::Depth, _depth)
        .mapForDraw({
            {SceneShader::ColorOutput, GL::Framebuffer::ColorAttachment{0}},
            {SceneShader::ObjectIdOutput, GL::Framebuffer::ColorAttachment{1}}});

    _cameraObject = new Object3D{&_scene};
    _camera = new SceneGraph::Camera3D{*_cameraObject};
    _camera->setAspectRatioPolicy(SceneGraph::AspectRatioPolicy::Extend);

    createGrid();

    loadResources();

    if(!_collectionConfig.hasGroup("scene"))
        _collectionConfig.addGroup("scene");

    _rootNode = Containers::pointer<ObjectNode>(&_scene, _collectionConfig.group("scene"));
    updateObjectNodeChildren(_rootNode.get());
    resetDrawablesId();

    _importer.createShaders(&_drawables, _lights.size(), shaderKeys, true);
}

void CollectionPanel::drawViewport(Float deltaTime) {
    /* If the window is not visible, end the method here. */
    if(!_isVisible || !_isOpen)
        return;

    _framebuffer
        .clearColor(0, Color3{0.12f})
        .clearColor(1, Vector4ui{0})
        .clearDepth(1.0f)
        .bind();

    if(_isSimulating) _scriptManager.update(deltaTime);

    for(std::size_t i = 0; i != _lights.size(); ++i)
        _lights[i].updateShader(*_camera, shaderKeys);

    _camera->draw(_drawables);
    _camera->draw(_editorDrawables);
}

void CollectionPanel::newFrame() {
    ImGui::SetNextWindowSizeConstraints(ImVec2(), ImVec2(_viewportTextureSize.x(),
        _viewportTextureSize.y()));

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    _isVisible = ImGui::Begin(_name.c_str(), &_isOpen, ImGuiWindowFlags_NoScrollbar |
        ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_MenuBar);
    ImGui::PopStyleVar();

    _isFocused = ImGui::IsWindowFocused();
    _isHovered = ImGui::IsWindowHovered();

    /* If the window is not visible, end the method here. */
    if(!_isVisible || !_isOpen) {
        ImGui::End();
        return;
    }

    if(ImGui::BeginMenuBar()) {
        const char* projections[] = {"Orthographic", "Perspective"};
        const char* currentProjection = projections[!_isOrthographicCamera];

        ImGui::PushItemWidth(120);
        if(ImGui::BeginCombo("##CameraProjection", currentProjection)) {
            for(auto& projection: projections) {
                bool isSelected = (currentProjection == projection);

                if(ImGui::Selectable(projection, isSelected)) {
                    _isOrthographicCamera = !_isOrthographicCamera;

                    Matrix4 currentCameraTransformation = _cameraObject->transformation();
                    _cameraObject->setTransformation(_prevCameraTransformation);
                    _prevCameraTransformation = currentCameraTransformation;
                }
                if(isSelected) ImGui::SetItemDefaultFocus();
            }

            ImGui::EndCombo();
        }

        ImGui::EndMenuBar();
    }

    const ImVec2 windowPos = ImGui::GetWindowPos();
    const ImVec2 windowSize = ImGui::GetWindowSize();

    _viewportPos = Vector2{windowPos.x, windowPos.y - (_viewportTextureSize.y() - windowSize.y)};
    ImGui::GetWindowDrawList()->AddImage(static_cast<ImTextureID>(&_viewportTexture), ImVec2(_viewportPos),
        ImVec2(_viewportPos + Vector2{_viewportTextureSize}), ImVec2(0, 1), ImVec2(1, 0));

    _viewportSize = {ImGui::GetWindowContentRegionMax().x - ImGui::GetWindowContentRegionMin().x,
        ImGui::GetWindowContentRegionMax().y - ImGui::GetWindowContentRegionMin().y};

    _framebuffer.setViewport({{}, Vector2i{_viewportSize*_dpiScaleRatio}});

    if(_isOrthographicCamera)
        _camera->setProjectionMatrix(Matrix4::orthographicProjection(_viewportSize, -1000.0f, 1000.0f));
    else
        _camera->setProjectionMatrix(Matrix4::perspectiveProjection(Deg{70.0f}, _viewportSize.aspectRatio(),
            0.05, 500.0f));

    _camera->setViewport(Vector2i{_viewportSize});

    ImGui::End();
}

void CollectionPanel::save() {
    _collectionConfig.save();
}

void CollectionPanel::loadResources() {
    if(!_collectionConfig.hasGroup("external_resources"))
        return;

    for(Utility::ConfigurationGroup* resource: _collectionConfig.group("external_resources")->groups("resource")) {
        std::string resourceType = resource->value("type");
        std::string resourcePath = resource->value("path");

        if(resourceType == "Texture2D") {
            GL::Texture2D texture = _importer.loadTexture(Utility::Directory::read(Utility::Directory::join(_projectPath, resourcePath)));
            _resourceManager.set(resourcePath, std::move(texture), ResourceDataState::Final, ResourcePolicy::ReferenceCounted);
        }
    }
}

void CollectionPanel::updateObjectNodeChildren(ObjectNode* node) {
    for(auto childConfig: node->objectConfig()->groups("child")) {
        Object3D* child = _importer.loadObject(childConfig, node->object(),
            &_drawables, &_scripts, &_lights);
        ObjectNode* childNode = node->addChild(child, childConfig);

        if(!_drawables.isEmpty() && child == &_drawables[_drawables.size() - 1].object())
            _drawablesNodes.push_back(childNode);

        Math::Vector3<Rad> rotationRadians = child->rotation().toEuler();
        childNode->setRotationDegree({Float{Deg{rotationRadians.x()}}, Float{Deg{rotationRadians.y()}},
            Float{Deg{rotationRadians.z()}}});

        updateObjectNodeChildren(childNode);
    }
}

void CollectionPanel::createGrid() {
    const Int size = 20;
    _gridObject = new Object3D{&_scene};
    _gridObject->rotateX({Deg{90}});

    Resource<GL::AbstractShaderProgram, SceneShader> shaderResource = _resourceManager.get<GL::AbstractShaderProgram, SceneShader>("editor");
    if(!shaderResource)
        _resourceManager.set<GL::AbstractShaderProgram>(shaderResource.key(), new SceneShader{SceneShader::Flag::ObjectId}, ResourceDataState::Mutable, ResourcePolicy::ReferenceCounted);

    Resource<GL::Mesh> meshResource = _resourceManager.get<GL::Mesh>("grid");
    if(!meshResource) {
        GL::Mesh glMesh = MeshTools::compile(Primitives::grid3DWireframe({size - 1, size - 1}));
        _resourceManager.set(meshResource.key(), std::move(glMesh), ResourceDataState::Final, ResourcePolicy::ReferenceCounted);
    }

    Mesh& mesh = _gridObject->addFeature<Mesh>(&_editorDrawables);
    mesh.setMesh(meshResource);
    mesh.setShader(shaderResource);
    mesh.setSize({size, size, size});
    mesh.setAmbientColor(Color3{0.3f});
}

void CollectionPanel::resetObjectAndChildren(ObjectNode* node) {
    _importer.resetObject(node->object(), node->objectConfig());

    for(auto& child: node->children())
        resetObjectAndChildren(child.get());
}

void CollectionPanel::updateShader(Mesh& mesh) {
    Resource<Magnum::GL::AbstractShaderProgram, SceneShader> shaderResource = _importer.createShader(mesh, _lights.size(), shaderKeys, true);
    mesh.setShader(shaderResource);
}

void CollectionPanel::recreateShaders() {
    for(std::pair<std::string, SceneShader::Flags>& key: shaderKeys) {
        _resourceManager.set<GL::AbstractShaderProgram>(key.first, new SceneShader{key.second, UnsignedInt(_lights.size())},
            ResourceDataState::Mutable, ResourcePolicy::ReferenceCounted);
    }
}

void CollectionPanel::resetLightsId() {
    for(std::size_t i = 0; i != _lights.size(); ++i)
        _lights[i].setId(i);
}

void CollectionPanel::addFeatureToObject(ObjectNode* objectNode, Utility::ConfigurationGroup* featureConfig) {
    SceneGraph::AbstractFeature3D* newFeature = _importer.loadFeature(featureConfig, objectNode->object(), &_drawables, &_scripts, &_lights);

    if(featureConfig->value("type") == "light")
        recreateShaders();
    else if(featureConfig->value("type") == "mesh") {
        Mesh& mesh = reinterpret_cast<Mesh&>(*newFeature);
        Resource<GL::AbstractShaderProgram, SceneShader> shaderResource = _importer.createShader(mesh, _lights.size(), shaderKeys, true);
        mesh.setObjectId(_drawables.size());
        mesh.setShader(shaderResource);
        _drawablesNodes.push_back(objectNode);
    }
}

void CollectionPanel::removeDrawableNode(ObjectNode* objectNode) {
    _drawablesNodes.erase(std::find_if(_drawablesNodes.begin(), _drawablesNodes.end(),
        [&objectNode](ObjectNode* n) { return n == objectNode; }));

    resetDrawablesId();
}

void CollectionPanel::resetDrawablesId() {
    for(std::size_t i = 0; i != _drawables.size(); ++i) {
        Mesh* mesh = dynamic_cast<Mesh*>(&_drawables[i]);
        if(mesh) { mesh->setObjectId(i + 1); }
    }
}

void CollectionPanel::startSimulation() {
    _scriptManager.loadScripts(_scripts);
    _isSimulating = true;
}

void CollectionPanel::stopSimulation() {
    _scriptManager.unloadScripts();
    resetObjectAndChildren(_rootNode.get());
    _isSimulating = false;
}
