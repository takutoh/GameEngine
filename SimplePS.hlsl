struct VSOutput
{
    float4 svpos : SV_POSITION; // ���_�V�F�[�_�[���痈�����W
    float4 color : COLOR;       // ���_�V�F�[�_�[���痈���F
};

float4 pixel(VSOutput input) : SV_TARGET
{
    return input.color; // �F�����̂܂ܕ\��
}
