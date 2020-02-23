void main()
{
    DiffuseColor = vec4(FragCoord.x/Resolution.x,FragCoord.y/Resolution.y,FragCoord.z, 1.0f);
}