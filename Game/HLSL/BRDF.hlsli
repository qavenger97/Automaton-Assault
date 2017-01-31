#ifndef _BRDF_H
#define _BRDF_H


// Normalized Distribution Function [GGX]
// 1 / PI is removed, pi division done on full light contribution sum
float GGX_PI(float a, float3 N, float3 M)
{
	float aSq = a * a;
	float N_M = saturate( dot( N, M ) );
	float term0 = (N_M * aSq - N_M) * N_M + 1;
	return aSq / (term0 * term0);
}
// GGX reference: http://www.cs.cornell.edu/~srm/publications/EGSR07-btdf.pdf

// Fresnel Function - [Schlick]
float3 Schlick( float3 F0, float3 V, float3 M )
{
	float V_M = saturate( dot( V, M ) );
	return F0 + (1.0 - F0) * pow( 1.0 - V_M, 5.0 );
}
// Schlick reference: http://www.cs.virginia.edu/~jdl/bib/appearance/analytic%20models/schlick94b.pdf


// Geometric Shadowing Function (Combined with the BRDF denominator) - [Smith-GGX]
float SmithGGXH( float N_V, float aSq )
{
	return N_V + sqrt( (N_V - N_V * aSq) * N_V + aSq );
}
float SmithGGX( float a, float N_V, float N_L )
{
	float aSq = a*a;
	return rcp( SmithGGXH(N_V, aSq) * SmithGGXH(N_L, aSq));
}
// GGX reference: http://www.cs.cornell.edu/~srm/publications/EGSR07-btdf.pdf
// Smith reference: (offline publication): B. Smith, Geometrical shadowing of a random rough surface,
// (cont'd) IEEE Transactions on Antennas and Propagation, Vol AP-15, No 5, pp. 668-671, 1967. 
// (cont'd) "Geometrical shadowing of a Random Rough Surface" (Published 1967)

// Cook-Torrance microfacet specular BRDF function implementation
float3 MicrofacetBRDF_PI( float3 F0, float roughness, float3 L, float3 V, float3 N, float N_V, float N_L )
{
	float3 M = normalize( V + L );
	float a = roughness * roughness;

	float D = GGX_PI( a, N, M );	
	float3 F = Schlick( F0, V, M );
	float G_DivDenom = SmithGGX( a, N_V, N_L );

	return D*F*G_DivDenom;
}
// Cook-Torrence reference: http://graphics.pixar.com/library/ReflectanceModel/paper.pdf

#endif