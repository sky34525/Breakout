#version 330 core
layout (location = 0) in vec4 vertex;

out vec2 TexCoords;

uniform bool chaos; //混沌
uniform bool confuse; //颠倒
uniform bool shake; //晃动
uniform float time;

void main()
{
    //gl_Position 是 GLSL 顶点着色器中的内置输出变量，用于设置顶点的最终位置
    gl_Position = vec4(vertex.xy, 0.0f, 1.0f);
    vec2 texture = vertex.zw;
    if(chaos)
    {
        float strength = 0.3;
        // 计算扰动后的纹理坐标
        // 由于该shader是应用于一个覆盖整个屏幕的四边形，改变纹理坐标的位置可以改变画面中图形的相对位置，而改变顶点位置会使整个画面移动
        vec2 pos = vec2(texture.x + sin(time) * strength, texture.y + cos(time) * strength); //整个画面会以圆形轨迹循环晃动/旋转
        TexCoords = pos;
    }
    else if(confuse)
    {
        TexCoords = vec2(1.0 - texture.x, 1.0 - texture.y);
    }
    else{
        TexCoords = texture;
    }
    if(shake){
        float strength = 0.01;
        gl_Position.x += cos(time * 10) * strength;
        gl_Position.y += cos(time * 15) * strength;
    }
}