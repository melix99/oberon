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

#include <Corrade/Utility/Resource.h>
#include <Magnum/GL/AbstractShaderProgram.h>

namespace Oberon {

using namespace Magnum;

class Shader: public GL::AbstractShaderProgram {
    public:
        enum: UnsignedInt {
            ColorOutput = 0,
            ObjectIdOutput = 1
        };

        explicit Shader(const UnsignedInt lightCount);

        Shader& setAmbientColor(const Color3& color);
        Shader& setDiffuseColor(const Color3& color);
        Shader& setSpecularColor(const Color3& color);
        Shader& setShininess(Float shininess);

        Shader& setObjectId(UnsignedInt id);

        Shader& setTransformationMatrix(const Matrix4& matrix);
        Shader& setNormalMatrix(const Matrix3x3& matrix);
        Shader& setProjectionMatrix(const Matrix4& matrix);

        Shader& setLightPosition(UnsignedInt id, const Vector3& position);
        Shader& setLightColor(UnsignedInt id, const Color3& color);

    private:
        UnsignedInt _lightCount;
        Int _transformationMatrixUniform,
            _projectionMatrixUniform,
            _normalMatrixUniform,
            _ambientColorUniform,
            _diffuseColorUniform,
            _specularColorUniform,
            _shininessUniform,
            _objectIdUniform,
            _lightPositionsUniform,
            _lightColorsUniform;
};

}