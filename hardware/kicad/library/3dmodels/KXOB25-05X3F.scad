difference(){
 
    color("#f3f3f3")
    cube([23,8,1.8]);

    // remove some block
    translate([0.4, 0.71, 0.1])
    for(j=[0:2]){

        translate([7.5*j, 0.0, 0.0])
        cube([7.2,6.59,1.8]);   
    }
    
    // remove space for + pin
    translate([-0.1, 0.3, -0.1])
    cube([2.1,7.4,1]);
      
    // remove space for - pin
    translate([21, 0.3, -0.1])
    cube([2.1,7.4,1]);
}

// add cells
color("#000000")
translate([0.4, 0.71, 0.1])

for(j=[0:2]){

    translate([7.5*j, 0.0, 0.0])
    cube([7.2,6.59,1.7]); 
}

// add + pin
color("#FAE481")
translate([0, 0.3, 0])
cube([2,7.4,0.9]);

// add - pin
color("#FAE481")
translate([21, 0.3, 0])
cube([2,7.4,0.9]);