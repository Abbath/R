#ifdef GL_ES
// Set default precision to medium
precision mediump int;
precision mediump float;
#endif

//uniform vec4 color;
varying vec3 color;
//! [0]
void main()
{
    // Set fragment color from texture
    gl_FragColor = vec4(color.r, color.g, color.b, 1.0);
}
//! [0]

