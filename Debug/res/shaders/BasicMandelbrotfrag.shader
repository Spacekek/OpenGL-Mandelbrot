#version 450 core
in vec4 gl_FragCoord;
 
out vec4 frag_color;
uniform float zoom;
uniform float x;
uniform float y;
uniform int color;
uniform int maximum_iterations;

vec3 spectral_color(float l)        // RGB <0,1> <- lambda l <400,700> [nm]
    {
    float t;  vec3 c=vec3(0.0,0.0,0.0);
         if ((l>=400.0)&&(l<410.0)) { t=(l-400.0)/(410.0-400.0); c.r=    +(0.33*t)-(0.20*t*t); }
    else if ((l>=410.0)&&(l<520.0)) { t=(l-410.0)/(520.0-410.0); c.r=0.14         -(0.13*t*t); }
    else if ((l>=545.0)&&(l<595.0)) { t=(l-545.0)/(595.0-545.0); c.r=    +(1.98*t)-(     t*t); }
    else if ((l>=595.0)&&(l<650.0)) { t=(l-595.0)/(650.0-595.0); c.r=0.98+(0.06*t)-(0.40*t*t); }
    else if ((l>=650.0)&&(l<700.0)) { t=(l-650.0)/(700.0-650.0); c.r=0.65-(0.84*t)+(0.20*t*t); }
         if ((l>=400.0)&&(l<520.0)) { t=(l-400.0)/(520.0-400.0); c.g=             +(0.80*t*t); }
    else if ((l>=520.0)&&(l<590.0)) { t=(l-520.0)/(590.0-520.0); c.g=0.8 +(0.76*t)-(0.80*t*t); }
    else if ((l>=585.0)&&(l<639.0)) { t=(l-585.0)/(639.0-585.0); c.g=0.84-(0.84*t)           ; }
         if ((l>=400.0)&&(l<520.0)) { t=(l-400.0)/(520.0-400.0); c.b=    +(2.20*t)-(1.50*t*t); }
    else if ((l>=520.0)&&(l<560.0)) { t=(l-520.0)/(560.0-520.0); c.b=0.7 -(     t)+(0.30*t*t); }
    return c;
    }

vec3 spectral_color2(float l)        // RGB <0,1>
    {
    float t=1.0;  vec3 c=vec3(0.0,0.0,0.0);
    c.r = t*l;
    c.g = t*(0.3-l);
    c.b = t*(1.0-l);
    if (l==1.0 || l==0.0){c=vec3(0.0,0.0,0.0);}
    return c;
    }

vec3 spectral_color3(float l)        // RGB <0,1>
    {
    float t=1.0;  vec3 c=vec3(0.0,0.0,0.0);
    c.r = t*(1.0-l);
    c.g = t*(l-0.2);
    c.b = t*l;
    if (l==1.0 || l==0.0){c=vec3(0.0,0.0,0.0);}
    return c;
    }

vec4 mandelbrot_iterations()
{
    float real = ((gl_FragCoord.x / 1400.0f - 0.5f) * zoom + x) * 4.0f;
    float imag = ((gl_FragCoord.y / 1400.0f - 0.5f) * zoom + y) * 4.0f;
 
    int iterations = 0;
    float const_real = real;
    float const_imag = imag;
    int n;
    int sh=7;
    float q;
    float dist;
    float tmp_real;

    while (iterations < maximum_iterations)
    {
        tmp_real = real;
        real = (real * real - imag * imag) + const_real;
        imag = (2.0f * tmp_real * imag) + const_imag;
         
        dist = real * real + imag * imag;
        if (dist > 4.0f)
            break;
        ++iterations;
    }
    tmp_real = real;
    real = (real * real - imag * imag) + const_real;
    imag = (2.0f * tmp_real * imag) + const_imag;
    tmp_real = real;
    real = (real * real - imag * imag) + const_real;
    imag = (2.0f * tmp_real * imag) + const_imag;
    ++iterations;
    ++iterations;
    dist = real * real + imag * imag;
    float rad_escape = iterations-log(log(sqrt(dist)))/log(2.0);
    rad_escape*=float(1<<sh); 
    int i=int(rad_escape);
    n=maximum_iterations<<sh;
    if (i>n) i=n;
    if (i<0) i=0;
    q=float(i)/float(n);
    q=pow(q,0.2);
    vec4 colour;
    if (color==0){colour=vec4(spectral_color(400.0+(300.0*q)),1.0);}
    else if(color==1){
	if (q==1.00){q=0.0;}
	vec3 c=vec3(q,q,q);
	colour=vec4(c,1.0);}
    else if(color==2){colour=vec4(spectral_color2(q),1.0);}
    else if(color==3){colour=vec4(spectral_color3(q),1.0);}
    return colour;
}

void main()
{
    frag_color = mandelbrot_iterations();
}