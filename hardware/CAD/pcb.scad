module encoder() {
    color([1.0, 0.15, 0.0])
        cube([20, 20, 1]);
    color([0.5, 0.5, 0.5])
        translate([10, 10, 1])
            cylinder(h=19, r=2, $fn=10);
}   

module lcd() {
    color([1, 1, 1])    
        translate([1, 7, 0])
            cube([3, 25, 10]);
    color([0.97, 0.58, 0.1])
        translate([0, 0, 10])
            cube([68, 40, 1]);
    color([0, 0, 0])
        translate([12.3, 3.5, 11])
            cube([45, 33, 2]);
}

module sensor() {
    color([0.94, 0.43, 0.69])
        cube([15, 19, 1]);   
}

module core() {
    color([0.43, 0.80, 0.96])
        cube([20, 36, 1]);
}

module supply() {
    color([1, 0.99, 0.04])
        cube([15, 41, 17]);
}

module relay() {
    color([0.5, 0.79, 0.61])
        cube([29, 53, 17]);
}

module assembled() {
    color([0.5, 0.57, 0])
        cube([70, 51, 1]);
    
    translate([2, 45, 1])
        encoder();
    
    translate([28, -19, 1])
        sensor();
    
    translate([8, -3, 1])
        core();
    
    translate([28, 5, 1])
        supply();  

    translate([43, -1, 1])
        relay();
    
    translate([1, 5.5, 11.5])
        lcd();
}

module pcb() {
    mirror([1,0,0])
        assembled();
}