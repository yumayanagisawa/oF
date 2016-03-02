uniform vec3      iResolution;           // viewport resolution (in pixels)
uniform float     iGlobalTime;           // shader playback time (in seconds)
uniform float     iTimeDelta;            // render time (in seconds)
uniform int       iFrame;                // shader playback frame
uniform float     iChannelTime[4];       // channel playback time (in seconds)
uniform vec3      iChannelResolution[4]; // channel resolution (in pixels)
uniform vec4      iMouse;                // mouse pixel coords. xy: current (if MLB down), zw: click

// fixed
//uniform samplerXX iChannel0..3;          // input channel. XX = 2D/Cube
uniform sampler2D iChannel0;
uniform sampler2D iChannel1;

//////////////////////////////////////////////////
// XBE
// Weird Warp
// Playing with domain warp to produce weird anim.

// Simplex Noise by IQ
vec2 hash( vec2 p )
{
    p = vec2( dot(p,vec2(127.1,311.7)),
             dot(p,vec2(269.5,183.3)) );
    
    return -1.0 + 2.0*fract(sin(p)*43758.5453123);
}

float noise( in vec2 p )
{
    const float K1 = 0.366025404; // (sqrt(3)-1)/2;
    const float K2 = 0.211324865; // (3-sqrt(3))/6;
    
    vec2 i = floor( p + (p.x+p.y)*K1 );
    
    vec2 a = p - i + (i.x+i.y)*K2;
    vec2 o = (a.x>a.y) ? vec2(1.0,0.0) : vec2(0.0,1.0); //vec2 of = 0.5 + 0.5*vec2(sign(a.x-a.y), sign(a.y-a.x));
    vec2 b = a - o + K2;
    vec2 c = a - 1.0 + 2.0*K2;
    
    vec3 h = max( 0.5-vec3(dot(a,a), dot(b,b), dot(c,c) ), 0.0 );
    
    vec3 n = h*h*h*h*vec3( dot(a,hash(i+0.0)), dot(b,hash(i+o)), dot(c,hash(i+1.0)));
    
    return dot( n, vec3(70.0) );
    
}

const mat2 m = mat2( 0.80,  0.60, -0.60,  0.80 );

float fbm4( in vec2 p )
{
    float f = 0.0;
    f += 0.5000*noise( p ); p = m*p*2.02;
    f += 0.2500*noise( p ); p = m*p*2.03;
    f += 0.1250*noise( p ); p = m*p*2.01;
    f += 0.0625*noise( p );
    return f;
}

float fbm6( in vec2 p )
{
    float f = 0.0;
    f += 0.5000*noise( p ); p = m*p*2.02;
    f += 0.2500*noise( p ); p = m*p*2.03;
    f += 0.1250*noise( p ); p = m*p*2.01;
    f += 0.0625*noise( p ); p = m*p*2.04;
    f += 0.031250*noise( p ); p = m*p*2.01;
    f += 0.015625*noise( p );
    return f;
}

float turb4( in vec2 p )
{
    float f = 0.0;
    f += 0.5000*abs(noise(p)); p = m*p*2.02;
    f += 0.2500*abs(noise(p)); p = m*p*2.03;
    f += 0.1250*abs(noise(p)); p = m*p*2.01;
    f += 0.0625*abs(noise(p));
    return f;
}

float turb6( in vec2 p )
{
    float f = 0.0;
    f += 0.5000*abs(noise(p)); p = m*p*2.02;
    f += 0.2500*abs(noise(p)); p = m*p*2.03;
    f += 0.1250*abs(noise(p)); p = m*p*2.01;
    f += 0.0625*abs(noise(p)); p = m*p*2.04;
    f += 0.031250*abs(noise(p)); p = m*p*2.01;
    f += 0.015625*abs(noise(p));
    return f;
}

float marble(in vec2 p)
{
    return cos(p.x+fbm4(p));
}

float wood(in vec2 p)
{
    float n = noise(p);
    return n-floor(n);
}

float dowarp ( in vec2 q, out vec2 a, out vec2 b )
{
    float ang = 1.5*texture2D(iChannel0, vec2(0.2, 0.)).x;
    mat2 rotmat = mat2(cos(ang), -sin(ang), sin(ang), cos(ang));
    a = vec2( marble(q), marble(q) );
    
    ang = 1.5*texture2D(iChannel0, vec2(0.6, 0.)).x;
    rotmat = mat2(cos(ang), -sin(ang), sin(ang), cos(ang));
    b = vec2( marble( rotmat*(q + a)), marble( rotmat*(q + a) ) );
    
    vec2 offset;
    offset.x = texture2D(iChannel0, vec2(0.1, 0.)).x;
    offset.y = texture2D(iChannel0, vec2(0.5, 0.)).x;
    return marble( q + b + 2.*offset);
}

// -----------------------------------------------

//void mainImage( out vec4 fragColor, in vec2 fragCoord )
void main()
{
    vec2 uv = gl_FragCoord.xy / iResolution.xy;
    vec2 q = 2.*uv-1.;
    q.y *= iResolution.y/iResolution.x;
    
    float Time = 0.25*(15.0 + iGlobalTime) - 1.*texture2D(iChannel0, vec2(0.65, 0.)).x;
    q += vec2( 4.0*cos(Time+45.), 2.0*sin(Time) );
    q *= .625 + 3.*texture2D(iChannel0, vec2(0.35, 0.)).x;
    
    vec2 a = vec2(0.);
    vec2 b = vec2(0.);
    float f = dowarp(q, a, b);
    f = 0.5+0.5*f;
    
    vec3 col = vec3(f);
    float c = 0.;
    c = f;
    col = vec3(c, c*c, c*c*c);
    c = fract(abs(a.x));
    col -= vec3(c*c, c, c*c*c);
    c = fract(abs(b.x));
    col += vec3(c*c*c, c*c, c);
    col *= 0.7;
    col.x = pow(col.x, 2.18);
    col.z = pow(col.z, 1.88);
    col = smoothstep(0., 1., col);
    col = sqrt(col);
    col = clamp(col, 0.,1.);
    
    // Vignetting
    vec2 r = -1.0 + 2.0*(uv);
    float vb = max(abs(r.x), abs(r.y));
    col *= (0.15 + 0.85*(1.0-exp(-(1.0-vb)*30.0)));
    gl_FragColor = vec4( col, 1.0 );
}