
void main()
{
    float v = 1.0f - length(MousePosition - FragCoord) / (Resolution.x / 5.0f);
    
    if(MousePosition.z > 0.0)
    {
        DiffuseColor = vec4(0.0,v,v,1.0f);
    }
    else if(MousePosition.w > 0.0)
    {
        DiffuseColor = vec4(v*abs(sin(TotalTime*2)),v*abs(cos(TotalTime)),v*abs(sin(TotalTime/2.0f)),1.0f);
    }
    else
    {
        DiffuseColor = vec4(v,v,v,1.0f);
    }
}