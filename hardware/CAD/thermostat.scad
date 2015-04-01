use <pcb.scad>;
use <involute_gears.scad>;

ring_inner_radius = 53;
ring_height = 20;
ring_number_of_teeth = 70;
fn = 50;
gear_thickness = 4;
gear_number_of_teeth = 15;
gear_offset = 5;

module ring() {
    difference() {
        cylinder(r=ring_inner_radius+0.5, h=ring_height, $fn=fn);
        translate([0, 0, -0.5])
            cylinder(r=ring_inner_radius, h=ring_height+1.0, $fn=fn);
    }

    translate([0, 0, gear_offset])
        difference() {
            cylinder(r=ring_inner_radius, h=gear_thickness, $fn=fn);
            translate([0, 0, -0.5])
                gear(circular_pitch=250,
                     number_of_teeth=ring_number_of_teeth,
                     bore_diameter=0,
                     gear_thickness=gear_thickness+1,
                     rim_thickness=gear_thickness+1,
                     hub_thickness=gear_thickness+1);
        }
}

module small_gear() {
    color([0, 1, 0])
        translate([23, 30, gear_offset])
            gear(circular_pitch=250,
                 number_of_teeth=gear_number_of_teeth,
                 gear_thickness=gear_thickness,
                 rim_thickness=gear_thickness,
                 hub_thickness=gear_thickness);
}

rotate([0, 0, 360 * $t * (gear_number_of_teeth / ring_number_of_teeth)])
    ring();

rotate([0, 0, 360 * $t + 47])
    small_gear();

translate([35, -25, 0])
    pcb();