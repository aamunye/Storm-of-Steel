rootnode = gr.node('root')
rootnode:rotate('y', 0.0)
rootnode:rotate('x', 40.0)
rootnode:scale( 0.25, 0.25, 0.25 )
rootnode:translate(0.0, 0.0, -1.0)

red = gr.material({1.0, 0.0, 0.0}, {0.1, 0.1, 0.1}, 10)
blue = gr.material({0.0, 0.0, 1.0}, {0.1, 0.1, 0.1}, 10)
green = gr.material({0.0, 1.0, 0.0}, {0.1, 0.1, 0.1}, 10)
white = gr.material({1.0, 1.0, 1.0}, {0.1, 0.1, 0.1}, 10)

purple = gr.material({0.2, 0.15, 0.42} , {0.1, 0.1, 0.1}, 10)

torso = gr.mesh('cube', 'torso')
rootnode:add_child(torso)
torso:set_material(green)
torso:scale(1.0,2.0,1.0);

undotorso = gr.node('undotorso')
undotorso:scale(1.0,1.0/2.0,1.0)
torso:add_child(undotorso)

neck = gr.mesh('sphere','neck')
undotorso:add_child(neck)
neck:set_material(purple)
neck:scale(0.25,0.25,0.25)
neck:translate(0.0,1.0,0.0)

undoneck = gr.node('undoneck')
undoneck:scale(1.0/0.25,1.0/0.25,1.0/0.25)
neck:add_child(undoneck)

headJoint = gr.joint('headJoint', {-500,0,500},{-500,0,500})
undoneck:add_child(headJoint)

head = gr.mesh('sphere','head')
head:scale(0.25,0.4,0.25)
headJoint:add_child(head)
head:set_material(white)
head:translate(0.0,0.4,0.0)

undohead = gr.node('undohead')
undohead:scale(1.0/0.25,1.0/0.4,1.0/0.25)
head:add_child(undohead)

nose = gr.mesh('sphere','nose')
nose:scale(0.1,0.1,0.1)
undohead:add_child(nose)
nose:set_material(green)
nose:translate(0.0,0.0,0.2)





---------------------------------------------------
---------------------RIGHT-------------------------
---------------------------------------------------

rightShoulder = gr.mesh('sphere', 'rightshoulder')
undotorso:add_child(rightShoulder)
rightShoulder:set_material(red)
rightShoulder:scale(0.25,0.25,0.25)
rightShoulder:translate(0.5,1.0,0.5)

undorightShoulder = gr.node('undorightShoulder')
undorightShoulder:scale(1.0,1.0,1.0)
rightShoulder:add_child(undorightShoulder)

rightUpperArmJoint = gr.joint('rightUpperArmJoint', {-500,0,500},{-500,0,500})
undorightShoulder:add_child(rightUpperArmJoint)

rightUpperArm = gr.mesh('cube','rightUpperArm')
rightUpperArmJoint:add_child(rightUpperArm)
rightUpperArm:set_material(blue)
rightUpperArm:scale(2,2*2.5,2)
rightUpperArm:translate(1.5,-2.5,0)

undorightUpperArm = gr.node('undorightUpperArm')
undorightUpperArm:scale(1.0,1.0/2.5,1.0)
rightUpperArm:add_child(undorightUpperArm)

--rightForearmJoint:rotate('z',30)

rightElbow = gr.mesh('sphere','rightElbow')
undorightUpperArm:add_child(rightElbow)
rightElbow:set_material(white)
rightElbow:scale(0.5,0.5,0.5)
rightElbow:translate(0,-1.5,0)

undorightElbow = gr.node('undorightElbow')
undorightElbow:scale(1.0,1.0,1.0)
rightElbow:add_child(undorightElbow)

rightLowerArmJoint = gr.joint('rightLowerArmJoint', {-500,0,500},{-500,0,500})
undorightElbow:add_child(rightLowerArmJoint)

rightLowerArm = gr.mesh('cube','rightLowerArm')
rightLowerArmJoint:add_child(rightLowerArm)
rightLowerArm:set_material(blue)
rightLowerArm:scale(2,2*2.5,2)
rightLowerArm:translate(0,-2.5,0)

--Arm--
----------------------------------------------------------------------------------------
--Leg--

rightHip = gr.mesh('sphere','rightHip')
undotorso:add_child(rightHip)
rightHip:set_material(purple)
rightHip:scale(1/5,1/5,1/5)
rightHip:translate(0.25,-1.0,0)

undorightHipJoint = gr.node('undorightHipJoint')
undorightHipJoint:scale(1.0,1.0,1.0)
rightHip:add_child(undorightHipJoint)

rightUpperLegJoint = gr.joint('rightUpperLegJoint', {-500,0,500},{-500,0,500})
rightHip:add_child(rightUpperLegJoint)

rightUpperLeg = gr.mesh('cube','rightUpperLeg')
rightUpperLegJoint:add_child(rightUpperLeg)
rightUpperLeg:set_material(red)
rightUpperLeg:scale(2,2*2.5,2)
rightUpperLeg:translate(0,-2.7,0)

undorightUpperLeg = gr.node('undorightUpperLeg')
undorightUpperLeg:scale(1.0,1.0/2.5,1.0)
rightUpperLeg:add_child(undorightUpperLeg)

rightKnee = gr.mesh('sphere','rightKnee')
undorightUpperLeg:add_child(rightKnee)
rightKnee:set_material(white)
rightKnee:scale(0.5,0.5,0.5)
rightKnee:translate(0,-1.5,0)

undorightKnee = gr.node('undorightKnee')
undorightKnee:scale(1.0,1.0,1.0)
rightKnee:add_child(undorightKnee)

rightLowerLegJoint = gr.joint('rightLowerLegJoint', {-500,0,500},{-500,0,500})
undorightKnee:add_child(rightLowerLegJoint)

rightLowerLeg = gr.mesh('cube','rightLowerLeg')
rightLowerLegJoint:add_child(rightLowerLeg)
rightLowerLeg:set_material(blue)
rightLowerLeg:scale(2,2*2.5,2)
rightLowerLeg:translate(0,-2.5,0)



return rootnode
