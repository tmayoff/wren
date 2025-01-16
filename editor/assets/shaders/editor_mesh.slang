struct Fragment
{
    float4 color;
};

[shader("fragment")]
Fragment fragment () {
    Fragment output;
    output.color = float4(1.0);
    return output;
}
