use <pcb.scad>;
use <involute_gears.scad>;

ring_inner_radius = 53;
ring_height = 19;
ring_number_of_teeth = 70;
fn = 50;
gear_thickness = 4;
gear_number_of_teeth = 15;
gear_offset = 5;
base_thickness = 1;
lid_radius = 200;

module ring() {
    translate([0, 0, base_thickness])
        difference() {
            cylinder(r=ring_inner_radius+0.5, h=ring_height, $fn=fn);
            translate([0, 0, -0.5])
                cylinder(r=ring_inner_radius, h=ring_height+1.0, $fn=fn);
        }

    translate([0, 0, gear_offset+base_thickness])
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
        translate([23, 30, gear_offset+base_thickness])
            rotate([0, 0, 360 * $t + 47])
                gear(circular_pitch=250,
                     number_of_teeth=gear_number_of_teeth,
                     gear_thickness=gear_thickness,
                     rim_thickness=gear_thickness,
                     hub_thickness=gear_thickness);
}

module lid() {
    color([0.5, 0.5, 0.5])
        difference() {
            translate([0, 0, -191.8+ring_height])
                difference() {
                    sphere(r=lid_radius, $fn=80);
                    sphere(r=lid_radius-0.5, $fn=80);
                    translate([0, 0, -7.3])
                        cube([lid_radius*2, lid_radius*2, lid_radius*2], center=true);
                }
            translate([0, 0, 0])
                cylinder(r=16, h=30, $fn=50);
        }
}

module top() {
    rotate([0, 0, 360 * $t * (gear_number_of_teeth / ring_number_of_teeth)])
        ring();
    
    lid();
}

top();
small_gear();

translate([35, -25.5, 0])
    pcb();