-- Debug
draw_features = true
draw_coastline = false
draw_bays = true
draw_structure = false

-- Silhouette
simplex_freq = 0.005
simplex_octaves = 4
simplex_lacunarity = 3.2
simplex_gain = 0.32
simplex_weighted_strength = 0.25
tier_land = 0.002
islands_to_keep = 4

-- Strucutre
coast_min_area = 100
coast_points_min_distance = 30
coast_points_max_distance = 100
min_land_distance_delta = 5
poisson_disk_sampling_radius = 10.0
coast_length = 53

-- River
min_source_mouth_distance_x = 30
min_source_mouth_distance_y = 100
river_noise_freq = 0.015
river_noise_octaves = 3
river_noise_lacunarity = 2.2
river_noise_gain = 0.7
river_noise_weighted_strength = 0.1
river_noise_weight = 40
