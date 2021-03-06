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

#define ANTIALIASING 2.0

#define VertexData _VertexData { \
    noperspective highp float edgeDistance; \
    noperspective highp float size; \
    smooth lowp vec4 color; \
}

in VertexData data;

out lowp vec4 fragmentColor;

void main() {
    fragmentColor = data.color;

    #ifdef LINES
    float d = abs(data.edgeDistance)/data.size;
    d = smoothstep(1.0, 1.0 - (ANTIALIASING/data.size), d);
    fragmentColor.a *= d;
    #else
    #ifdef POINTS
    float d = length(gl_PointCoord.xy - vec2(0.5));
    d = smoothstep(0.5, 0.5 - (ANTIALIASING/data.size), d);
    fragmentColor.a *= d;
    #endif
    #endif
}
