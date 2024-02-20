Shader "VideoShader"
{
    Properties
    {
        _MainTex ("Base (RGB) Trans (A)", 2D) = "white" {}
        _Color ("Color", Color) = (1,1,1,1)
        _Hue ("Hue", Range(-360, 360)) = 0.
        _Brightness ("Brightness", Range(0, 2)) = 1
        _Contrast("Contrast", Range(0, 2)) = 1
        _Saturation("Saturation", Range(0, 2)) = 1
        _Exposure("Exposure", Range(0, 2)) = 1
        _Gamma("Gamma", Range(0, 2)) = 1
        _Shadows("Shadows", Range(1, 2)) = 1 
        _Midtones("Midtones", Range(0, 2)) = 1
        _Highlights("Highlights", Range(0, 2)) = 1
        _VignetteRadius("VignetteRadius", Range(0, 1)) = 1
        _VignetteSoftness("VignetteSoftness", Range(0, 1)) = 0
        _VignetteOval("VignetteOval", Int) = 0
        _BlendOp("Blend Operation", Float) = 0
        _SrcColor("Blend SrcColor", Float) = 1
        _DestColor("Blend DestColor", Float) = 0
        _SrcAlpha("Blend SrcAlpha", Float) = 0
        _DestAlpha("Blend DestAlpha", Float) = 1
    }

    SubShader
    {
        Tags
        {
            "Queue"="Transparent" "IgnoreProjector"="True" "RenderType"="Transparent" "PreviewType"="Plane" "CanUseSpriteAtlas"="True"
        }
        LOD 100

        BlendOp [_BlendOp]
        Blend [_SrcColor] [_DestColor], [_SrcAlpha] [_DestAlpha]
        
        //BlendOp Add
        //Blend OneMinusDstColor One //Default Cinema blend mode
        //Blend OneMinusDstColor One, OneMinusDstColor One //Default Cinema blend mode
        //Blend One OneMinusSrcColor, OneMinusDstAlpha One //The same result as above?

        ZWrite On
        //AlphaTest Greater 0.01

        Pass
        {
            CGPROGRAM
            #pragma vertex vert
            #pragma fragment frag
            
            #include "UnityCG.cginc"
            
            struct appdata_t {
                float4 vertex : POSITION;
                float2 texcoord : TEXCOORD0;

                UNITY_VERTEX_INPUT_INSTANCE_ID 
            };
            
            struct v2f {
                float4 vertex : SV_POSITION;
                half2 texcoord : TEXCOORD0;
                float3 vs_TEXCOORD1 : TEXCOORD1;

                UNITY_VERTEX_OUTPUT_STEREO
            };
            
            sampler2D _CinemaVideoTexture;
            float4 _CinemaVideoTexture_ST;
            float4 _Color;
            float _Hue;
            float _Brightness;
            float _Contrast;
            float _Saturation;
            float _Exposure;
            float _Gamma;
            float _Shadows;
            float _Midtones;
            float _Highlights;
            float _VignetteRadius;
            float _VignetteSoftness;
            int _VignetteOval;
            
            v2f vert (appdata_t v)
            {
                v2f o;

                UNITY_SETUP_INSTANCE_ID(v);
                UNITY_INITIALIZE_OUTPUT(v2f, o);
                UNITY_INITIALIZE_VERTEX_OUTPUT_STEREO(o);
                
                o.vertex = UnityObjectToClipPos(v.vertex);
                o.texcoord = TRANSFORM_TEX(v.texcoord, _CinemaVideoTexture);
                o.vs_TEXCOORD1 = mul(unity_ObjectToWorld, v.vertex);
                return o;
            }
            
            inline float3 apply_hue(const float3 in_color, const float hue)
            {
                const float angle = radians(hue);
                const float3 k = float3(0.57735, 0.57735, 0.57735);
                const float cos_angle = cos(angle);
                //Rodrigues' rotation formula
                return in_color * cos_angle + cross(k, in_color) * sin(angle) + k * dot(k, in_color) * (1 - cos_angle);
            }

            //Function adapted from a shader by Christian Bloch (HDRLabs.com)
            inline float3 apply_exposure_and_gamma(const float3 in_color, const float exposure, const float gamma, const float shadows, const float midtones, const float highlights)
            {
                float3 out_color = (highlights/2 + 0.5) * pow(in_color * exposure + (shadows-1) * (1-in_color) * 0.5, 1/(gamma * midtones));
                return out_color;
            }
                 
            inline float4 apply_hsb_effect(const float4 start_color)
            {
                float4 output_color = start_color;
                
                output_color.rgb = apply_exposure_and_gamma(output_color.rgb, _Exposure, _Gamma, _Shadows, _Midtones, _Highlights);
                output_color.rgb = (output_color.rgb - 0.5f) * (_Contrast)+0.5f;
                output_color.rgb = output_color.rgb + (_Brightness - 1.0f);
                const float3 intensity = dot(output_color.rgb, float3(0.299, 0.587, 0.114));
                output_color.rgb = lerp(intensity, output_color.rgb, _Saturation);
				output_color.rgb = apply_hue(output_color.rgb, _Hue);
                return output_color;
            }

			inline float4 apply_vignette(float4 in_color, v2f i)
			{
                if (_VignetteOval == 1) {
				    _VignetteRadius = clamp(_VignetteRadius, 0.05, 1);
                    const float max_radius = 0.0625;
                    const float radius = max_radius - (_VignetteRadius * max_radius);
                    const float smoothness = radius + ((_VignetteSoftness/5) * max_radius); //Dividing by 5 makes smoothness a bit less sensitive

                    float2 pos = i.texcoord.xy;
                    pos = (pos - .5) * sqrt(_VignetteRadius) + .5;
                    const float vignette = pos.x * pos.y * (1.-pos.x) * (1.-pos.y);
                    in_color.rgb = in_color.rgb * smoothstep(radius, smoothness, vignette);
                    return in_color;
                }

                const float2 uv = i.texcoord.xy;
                float2 dist_v = abs(uv-0.5)*2;
                const float max_dist  = max(abs(dist_v.x), abs(dist_v.y));
                const float circular = length(dist_v);
                const float square   = max_dist;

                const float color1 = 0.0;
                const float color2 = 1.0;
                float mate = lerp(color1, color2, lerp(circular,square,max_dist));
                mate = smoothstep(_VignetteRadius, _VignetteRadius - _VignetteSoftness, mate);
                return mate * in_color;
			}
            
            fixed4 frag (const v2f i) : COLOR
            {                
                float4 color = _Color * tex2D(_CinemaVideoTexture, i.texcoord);
                color = apply_hsb_effect(color);
				color = apply_vignette(color, i);
				color = saturate(color);
                color.a = 0;
                return color;
            }
            ENDCG
        }
    }
}