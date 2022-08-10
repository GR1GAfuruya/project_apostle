struct VS_OUT
{
    float4 position : SV_POSITION;      //vertexShaderは必ずこのセマンティックの値を出力しなければならない！！！！！   （値 = 頂点のスクリーン上の位置（ＮＤＣ）
    float4 color : COLOR;
    float2 texcoord : TEXCOORD;     //UV座標
};

