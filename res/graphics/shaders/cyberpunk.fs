#version 330 core

// --------------------------------------------------------------------------------------
// GDD v3.6.1 - "高级赛博朋克" P1 滤镜 (片段着色器)
// 效果： 1. 青色/品红 色调
//       2. 暗角 (Vignette)
//       3. 增强对比度
//       4. 拉低饱和度
// --------------------------------------------------------------------------------------

// "输入" (来自 Raylib 默认的顶点着色器)
in vec2 fragTexCoord; // 像素在贴图上的坐标 (0.0 -> 1.0)
in vec4 fragColor;    // 顶点颜色 (我们通常用 WHITE)

// "输出" (到屏幕)
out vec4 finalColor;

// "GDD 合同" (来自 C++ 的"全局变量")
uniform sampler2D texture0; // Raylib 默认使用 texture0

void main()
{
    // 1. 从"渲染纹理"中"采样"当前像素的"原始"颜色
    vec4 color = texture(texture0, fragTexCoord);
    
    // 【【"P1 施工"：效果 1 - 轻微深色化 + 保留原色 + 青绿霓虹点缀】】
    float luma = dot(color.rgb, vec3(0.299, 0.587, 0.114)); // 计算亮度
    
    // 轻微降低整体亮度，营造氛围（而非完全变暗）
    color.rgb *= 0.75; // 降低25%亮度（稍微暗一点）
    
    // 青绿霓虹色调 - 只在高光区域轻微应用
    vec3 neonTeal = vec3(0.2, 1.3, 1.4); // 青绿霓虹
    
    // 只对原色稍微染色（混合比例很低，保留原色）
    // 高亮区域多一些青绿色，暗区保持原色
    float tintStrength = luma * 0.25; // 只对亮区染色，且强度只有25%
    color.rgb = mix(color.rgb, color.rgb * neonTeal, tintStrength);


    // 【【"P1 施工"：效果 2 - 适度对比度（增强层次感）】】
    float contrast = 1.25; // 适度对比度
    color.rgb = ((color.rgb - 0.5) * contrast) + 0.5;


    // 【【"P1 施工"：效果 3 - 轻微暗角（营造氛围）】】
    vec2 uv = fragTexCoord - 0.5;
    float dist = 1.0 - (length(uv) * 0.5); // 暗角强度0.5
    dist = clamp(dist, 0.5, 1.0);          // 最暗处0.5
    color.rgb *= dist;


    // "输出"最终颜色
    finalColor = color * fragColor;
}
