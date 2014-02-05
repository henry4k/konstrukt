function SetLight( ambient, diffuse, direction )
{
    ::native.SetLight(
        ambient.r.tofloat(),
        ambient.g.tofloat(),
        ambient.b.tofloat(),

        diffuse.r.tofloat(),
        diffuse.g.tofloat(),
        diffuse.b.tofloat(),

        direction.x.tofloat(),
        direction.y.tofloat(),
        direction.z.tofloat()
    )
}

return {
    SetLight = SetLight
}
