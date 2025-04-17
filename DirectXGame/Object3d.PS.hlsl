struct PixcelShaderOutput {
    float4 color : SV_Target0;
};

PixcelShaderOutput main() {
    PixcelShaderOutput output;
    output.color = float1x4(1.0f, 1.0f, 1.0f, 1.0f); // White
    return output;
}
