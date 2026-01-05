static f32
lerp(f32 a, f32 b, f32 t)
{
    return a * (1.0f - t) + b * t;
}



static f32
damp(f32 a, f32 b, f32 k, f32 dt)
{
    return lerp(a, b, 1 - expf(-k * dt));
}



static f32
norm_of(f32 x, f32 y)
{
    f32 norm = sqrtf(x * x + y * y);
    return norm;
}



static f32
normalize(f32* x, f32* y)
{

    f32 x_value = *x;
    f32 y_value = *y;
    f32 norm    = norm_of(x_value, y_value);

    if (norm < 0.00001f) // Arbitrary threshold.
    {
        norm = 0.0f;
        *x   = 0.0f;
        *y   = 0.0f;
    }
    else
    {
        *x = x_value / norm;
        *y = y_value / norm;
    }

    return norm;

}
