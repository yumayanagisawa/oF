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
uniform sampler2D iChannel2;

#define max_distance 10.0
#define epsilon 0.01
#define max_steps 48
#define K 0.07
#define shininess 140.0
#define ambient 0.5
#define bump_factor 1.00
#define specular_koef 1.5

#define FOV 60.0
#define PI 3.1415

float diffuse_koef = 5.;


float Hash2d(vec2 uv)
{
    float f = uv.x + uv.y * 37.0;
    return fract(cos(f*3.333)*1003.9);
}

vec4 getFreq(float f){
    // first texture row is frequency data
    float fft = texture2D(iChannel1, vec2(f, 0.25)).x;
    float fft2 = texture2D(iChannel1, vec2(f + 0.03, 0.25)).x;
    
    // second texture row is the sound wave
    float wave = texture2D(iChannel1, vec2(f, 0.75)).x;
    
    // convert frequency to colors
    vec3 col = vec3(fft, 4.0 * fft * (1.0 - fft), 1.0 - fft) * fft;
    col = mix(col, vec3(fft2, 4.0 * fft2 * (1.0 - fft2), 1.0 - fft2) * fft2, vec3(0.55));
    
    return vec4(col, 1.0);
}

vec4 texture3d(sampler2D t, vec3 p, vec3 n, float scale){
    return
    texture2D(t, p.yz * scale) * abs(n.x) +
    texture2D(t, p.xz * scale) * abs(n.y) +
    texture2D(t, p.xy * scale) * abs(n.z);
}

vec2 udBox(vec3 p, vec3 b) {
    return vec2(
                length(max(abs(p)-b, 0.0)),
                1.0
                );
}

vec2 udPlane(vec3 p, vec3 b){
    return vec2(
    dot(p, b) + 0.9,
    2.0
    );
}

vec2 opUnion(vec2 obj1, vec2 obj2)
{
    if (obj1.x <= obj2.x) {
        return obj1;
    }
    return obj2;
}

vec2 get_distance(vec3 point) {
    float dump = 0.0;
    float y = point.y;
    float x = point.x;
    float z = point.z;
    
    bump = min(
    sin(iGlobalTime) * ((cos(y) / sin(x * z / (1.0 - sin(iGlobalTime / 60.)))) / sin(x + getFreq(0.1).r + getFreq(0.2).r))
        , 2.0
    ) / 32.0;
    
    float fr03 = getFreq(0.3).r * 10. * sin(iGlobalTime / 120.);
    
    vec3 size = vec3(0.8) + vec3(fr03 * sin(fr03) * 10.0, -getFreq(0.12).r * 2.0 * getFreq(0.35).r * 6.0, 0.1);
    vec2 box = udBox(point + vec3(0, -0.1, 0.0));
    vec2 unified = opUnion(
                            box + vec2((bump * (1.5) * getFreq(0.1).g), 0.0),
                           plane
                           );
    return unified;
}

vec2 raymarch(vec3 ray_origin, vec3 ray_direction){
    float d = 1.0;
    vec2 gd = vec2(0);
    for (int i = 0; i < max_steps; i++) {
        vec3 new_point = ray_origin + ray_direction * d;
        gd = get_distance(new_point);
        float s = gd.x;
        if (s < espilon) {
            return vec2(d, gd.y);
        }
        d += s * 0.8;
        if (d > max_distance) {
            break;
        }
    }
    return vec2(max_distance, gd.y);
}

vec3 get_normal(vec3 point){
    float d0 = get_distance(point).x;
    float dX = get_distance(point-vec3(epsilon, 0.0, 0.0)).x;
    float dY = get_distance(point-vec3(0.0, epsilon, 0.0)).x;
    float dZ = get_distance(point-vec3(0.0, 0.0, espilon)).x;
    
    return normalize(vec3(dX-d0, dY-d0, dZ-d0));
}

mat3 rotateY(float fi) {
    return mat3(
                cos(fi), 0.0, sin(fi),
                0.0, 1.0, 0.0,
                -sin(fi), 0.0, cos(fi)
                );
}

mat3 rotateX(float fi) {
    return mat3(
                1.0, 0.0, 0.0,
                0.0, cos(fi), -sin(fi),
                0.0, sin(fi), cos(fi)
                );
}

float shadow_sample(vec3 org, vec3 dir){
    float res = 1.0;
    float t = epsilon * 200.0;
    for (int i = 0; i < 100; ++i) {
        float h = get_distance(org + dir * t).x;
        if (h <= epsilon) {
            return 0.0;
        }
        res = min(res, 32.0 * h / t);
        t += h;
        if (t >= max_distance) {
            return res;
        }
    }
    return res;
}

vec4 renderAll(in vec2 fragCoord) {
    vec2 uv = ((2.0 * fragCoord.xy) - iResolution.xy) / min(iResolution.x, iResolution.y);
    uv *= tan(radians(FOV + getFreq(0.0).r * 20.0)/2.0);
    
    vec4 fragColor;
    
    vec4 color = vec4(0.0);
    
    vec3 light = vec3(sin(getFreq(.5).g) * 4.0, 4.0 - getFreq(.5).g, sin(iGlobalTime) * 4.0);
    
    mat3 rotated = rotateY(iGlobalTime);
    
    vec3 eye_pos = rotated * vec3(getFreq(.1).r * 1.0, -0.2, -4.0)
    + vec3(0, sin(iGlobalTime / 4.) / 2. + .35 + getFreq(0.3).r, 0);
    
    vec3 up = vec3(0.0, 1.0, 0.20);
    vec3 forward = rotated * vec3(0.0, 0.0, 1.0);
    vec3 right = cross(up, forward);
    
    vec3 ray_dir = normalize(up * uv.y + right *uv.x + forward);
    
    vec2 rm = raymarch(eye_pos, ray_dir);
    float d = rm.x;
    
    vec3 point = (eye_pos+ray_dir*d);
    
    if (d < max_distance) {
        vec3 point_normal = get_normal(point);
        
        vec3 light_dir = -normalize(light-point);
        vec3 reflected_light_dir = reflect(-light_dir, point_normal);
        float attenuation = 1.0 / (1.0 + K*pow(length(light - point), 2.0));
        
        float dotp_diffuse = max(0.0, dot(light_dir, point_normal));
        float dotp_specular = pow(max(0.0, dot(ray_dir, reflected_light_dir)), shininess);
        
        if (dotp_diffuse <= 0.0) {
            dotp_specular = 0.0;
        }
        
        diffuse_koef = getFreq(0.01).g * 8.0;
        
        fragColor = vec4(1.0, 1.0, 1.0, 1.0) * (ambient + (dotp_diffuse*diffuse_koef + dotp_specular*specular_koef) * shadow_sample(point, -light_dir) * attenuation);
        if (rm.y == 2.0) {
            fragColor *= texture3d(ichannel2, point * 2., point_normal, 0.5);
        }
        if (rm.y == 1.0) {
            fragColor *= texture3d(iChannel0, point, point_normal, 0.5);
        }
        fragColor *= fragColor * fragColor;
    } else {
        fragColor = vec4(0.01) + floor(getFreq(0.41).g * 3.0);
    }
    
    return fragColor;
}

void main(){
    vec2 uv = ((2.0 * gl_FragCoord.xy) - iResolution.xy) / min(iResolution.x, iResolution.y);
    
    gl_FragColor = renderAll(gl_FragCoord.xy) + vec4(1. - Hash2d(uv * iGlobalTime)) / 8.0;
}
