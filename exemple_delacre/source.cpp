void main() {
    uint x = gl_GlobalInvocationID.x;
    uint y = gl_GlobalInvocationID.y;
    uint p = x + y * WSX;

    // Délai avant démarrage (2 secondes à ~60fps)
    int delay = 120;
    int delay_vibraqua = delay + 180; // Lance-Flammes démarre après ~4 secondes (240 frames total)

    // --- Vibrobscur (data_0) ---
    if (step < delay) {
        data_0[p] = 0x00000000; // Transparent
    } else {
        // Coordonnées normalisées
        float fx = float(x) / WSX * 2.0 - 1.0;
        float fy = float(y) / WSY * 2.0 - 1.0;

        // Temps d'animation pour Vibrobscur
        float time = float(step - delay) * 0.02;
        float max_radius = 0.6;
        float base_radius = min(0.1 + time, max_radius);

        // Ondulation sinusoïdale avec plus de variabilité pour les bords
        float wave = 0.03 * sin(10.0 * fx + time * 5.0) * cos(10.0 * fy + time * 5.0);
        wave += 0.05 * sin(20.0 * fx + time * 3.0); // Déformation supplémentaire
        float radius = base_radius + wave;

        // Masque de transition (bords plus chaotiques)
        float dist = sqrt(fx * fx + fy * fy);
        float mask = smoothstep(radius, radius - 0.05 - wave * 0.5, dist);

        // Effet de distorsion dynamique
        float distortion = sin(time * 3.0 + fx * 10.0) * cos(time * 3.0 + fy * 10.0);
        int color_shift = int((distortion + 1.0) * 0.5 * 2.0) % 2; // Change entre 0 et 1
        int color = color_shift == 0 ? 0x000000 : 0x880088; // Noir ou violet foncé

        int alpha = int(mask * 255.0) << 24; // Opacité progressive

        // Stockage dans data_0 pour Vibrobscur
        data_0[p] = alpha + color;
    }

    // --- Vibraqua (data_1) ---
    if (step < delay_vibraqua) {
        data_1[p] = 0x00000000; // Transparent
    } else {
        // Coordonnées normalisées
        float fx = float(x) / WSX * 2.0 - 1.0;
        float fy = float(y) / WSY * 2.0 - 1.0;

        // Temps d'animation pour Vibraqua
        float time = float(step - delay_vibraqua) * 0.03; // Réduire la vitesse de l'attaque

        // L'attaque commence par un rayon plus petit et s'agrandit plus lentement
        float max_radius = 1.0; // Taille finale plus petite
        float base_radius = min(0.1 + time * 0.35, max_radius); // L'attaque grandit plus lentement

        // Forme allongée pour simuler l'eau, avec une distorsion dynamique
        float dist = sqrt(fx * fx + fy * fy); // Distance au centre
        float angle = atan(fy, fx); // Calculer l'angle pour donner une forme conique

        // Masque de transition plus large avec distorsion
        float mask = smoothstep(base_radius, base_radius - 0.05, dist) * (1.0 - 0.2 * abs(angle));

        // Couleurs
        int red   = int(mask * (0xFF));  
        int green = int(mask * (0x50));  
        int blue  = 0;                  

        int alpha = int(mask * 255.0) << 24; // Opacité progressive

        // Effet de distorsion dynamique 
        float distortion = sin(time * 8.0 + fx * 10.0) * cos(time * 8.0 + fy * 10.0);
        red = int(float(red) * (1.0 + distortion * 0.5));  
        green = int(float(green) * (1.0 + distortion * 0.2));  

        // Stockage dans data_1 pour Vibraqua
        int color = (red << 16) + (green << 8) + blue;
        data_1[p] = alpha + color;
    }
}
