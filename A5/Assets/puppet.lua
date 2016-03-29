rootnode = gr.node('root')
rootnode:rotate('y', 90.0)
--rootnode:rotate('x', 40.0)
--rootnode:rotate('z', 90.0)
rootnode:scale( 0.25, 0.25, 0.25 )
rootnode:translate(0.25, 0.0, 2.5)

red = gr.material({1.0, 0.0, 0.0}, {0.1, 0.1, 0.1}, 10)
blue = gr.material({0.0, 0.0, 1.0}, {0.1, 0.1, 0.1}, 10)
green = gr.material({0.0, 1.0, 0.0}, {0.1, 0.1, 0.1}, 10)
white = gr.material({1.0, 1.0, 1.0}, {0.1, 0.1, 0.1}, 10)
black = gr.material({0.0, 0.0, 0.0}, {0.1, 0.1, 0.1}, 10)

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

undorightLowerArm = gr.node('undorightLowerArm')
undorightLowerArm:scale(1.0,1.0/2.5,1.0)
rightLowerArm:add_child(undorightLowerArm)

rightWrist = gr.mesh('sphere','rightWrist')
undorightLowerArm:add_child(rightWrist)
rightWrist:set_material(white)
rightWrist:scale(0.5,0.5,0.5)
rightWrist:translate(0,-1.0,0)

undorightWrist = gr.node('undorightWrist')
undorightWrist:scale(1.0,1.0,1.0)
rightWrist:add_child(undorightWrist)

rightWristJoint = gr.joint('rightWristJoint', {-500,0,500},{-500,0,500})
undorightWrist:add_child(rightWristJoint)

rightHand = gr.mesh('suzanne','rightHand')
rightWristJoint:add_child(rightHand)
rightHand:set_material(purple)
rightHand:scale(1,1,1)
rightHand:translate(0,-1.5,0)

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

undorightLowerLeg = gr.node('undorightLowerLeg')
undorightLowerLeg:scale(1.0,1.0/2.5,1.0)
rightLowerLeg:add_child(undorightLowerLeg)

rightAnkle = gr.mesh('sphere','rightAnkle')
undorightLowerLeg:add_child(rightAnkle)
rightAnkle:set_material(white)
rightAnkle:scale(0.5,0.5,0.5)
rightAnkle:translate(0,-1.2,0)

undorightAnkle = gr.node('undorightAnkle')
undorightAnkle:scale(1.0,1.0/2.5,1.0)
rightAnkle:add_child(undorightAnkle)

rightFootJoint = gr.joint('rightFootJoint', {-500,0,500},{-500,0,500})
undorightAnkle:add_child(rightFootJoint)

rightFoot = gr.mesh('cube','rightFoot')
rightFootJoint:add_child(rightFoot)
rightFoot:set_material(blue)
rightFoot:scale(2.0,2.0,4.0)
rightFoot:translate(0.0,-1.7,1.0)

undorightFoot = gr.node('undorightFoot')
undorightFoot:scale(1.0/2.0,1.0/2.0,1.0/4.0)
rightFoot:add_child(undorightFoot)

rightToes = gr.mesh('suzanne','rightToes')
undorightFoot:add_child(rightToes)
rightToes:set_material(red)
rightToes:scale(1.0,2.0,1.0)
rightToes:translate(0.0,2.0,2.0)

----------------------------------------------------------------------------------------


---------------------------------------------------
---------------------LEFT-------------------------
---------------------------------------------------

leftShoulder = gr.mesh('sphere', 'leftshoulder')
undotorso:add_child(leftShoulder)
leftShoulder:set_material(red)
leftShoulder:scale(0.25,0.25,0.25)
leftShoulder:translate(-0.5,1.0,0.5)

undoleftShoulder = gr.node('undoleftShoulder')
undoleftShoulder:scale(1.0,1.0,1.0)
leftShoulder:add_child(undoleftShoulder)

leftUpperArmJoint = gr.joint('leftUpperArmJoint', {-500,0,500},{-500,0,500})
undoleftShoulder:add_child(leftUpperArmJoint)

leftUpperArm = gr.mesh('cube','leftUpperArm')
leftUpperArmJoint:add_child(leftUpperArm)
leftUpperArm:set_material(blue)
leftUpperArm:scale(2,2*2.5,2)
leftUpperArm:translate(-1.5,-2.5,0)

undoleftUpperArm = gr.node('undoleftUpperArm')
undoleftUpperArm:scale(1.0,1.0/2.5,1.0)
leftUpperArm:add_child(undoleftUpperArm)

--leftForearmJoint:rotate('z',30)

leftElbow = gr.mesh('sphere','leftElbow')
undoleftUpperArm:add_child(leftElbow)
leftElbow:set_material(white)
leftElbow:scale(0.5,0.5,0.5)
leftElbow:translate(0,-1.5,0)

undoleftElbow = gr.node('undoleftElbow')
undoleftElbow:scale(1.0,1.0,1.0)
leftElbow:add_child(undoleftElbow)

leftLowerArmJoint = gr.joint('leftLowerArmJoint', {-500,0,500},{-500,0,500})
undoleftElbow:add_child(leftLowerArmJoint)

leftLowerArm = gr.mesh('cube','leftLowerArm')
leftLowerArmJoint:add_child(leftLowerArm)
leftLowerArm:set_material(blue)
leftLowerArm:scale(2,2*2.5,2)
leftLowerArm:translate(0,-2.5,0)

undoleftLowerArm = gr.node('undoleftLowerArm')
undoleftLowerArm:scale(1.0,1.0/2.5,1.0)
leftLowerArm:add_child(undoleftLowerArm)

leftWrist = gr.mesh('sphere','leftWrist')
undoleftLowerArm:add_child(leftWrist)
leftWrist:set_material(white)
leftWrist:scale(0.5,0.5,0.5)
leftWrist:translate(0,-1.0,0)

undoleftWrist = gr.node('undoleftWrist')
undoleftWrist:scale(1.0,1.0,1.0)
leftWrist:add_child(undoleftWrist)

leftWristJoint = gr.joint('leftWristJoint', {-500,0,500},{-500,0,500})
undoleftWrist:add_child(leftWristJoint)

leftHand = gr.mesh('suzanne','leftHand')
leftWristJoint:add_child(leftHand)
leftHand:set_material(purple)
leftHand:scale(1,1,1)
leftHand:translate(0,-1.5,0)

--Arm--
----------------------------------------------------------------------------------------
--Leg--

leftHip = gr.mesh('sphere','leftHip')
undotorso:add_child(leftHip)
leftHip:set_material(purple)
leftHip:scale(1/5,1/5,1/5)
leftHip:translate(-0.25,-1.0,0)

undoleftHipJoint = gr.node('undoleftHipJoint')
undoleftHipJoint:scale(1.0,1.0,1.0)
leftHip:add_child(undoleftHipJoint)

leftUpperLegJoint = gr.joint('leftUpperLegJoint', {-500,0,500},{-500,0,500})
leftHip:add_child(leftUpperLegJoint)

leftUpperLeg = gr.mesh('cube','leftUpperLeg')
leftUpperLegJoint:add_child(leftUpperLeg)
leftUpperLeg:set_material(red)
leftUpperLeg:scale(2,2*2.5,2)
leftUpperLeg:translate(0,-2.7,0)

undoleftUpperLeg = gr.node('undoleftUpperLeg')
undoleftUpperLeg:scale(1.0,1.0/2.5,1.0)
leftUpperLeg:add_child(undoleftUpperLeg)

leftKnee = gr.mesh('sphere','leftKnee')
undoleftUpperLeg:add_child(leftKnee)
leftKnee:set_material(white)
leftKnee:scale(0.5,0.5,0.5)
leftKnee:translate(0,-1.5,0)

undoleftKnee = gr.node('undoleftKnee')
undoleftKnee:scale(1.0,1.0,1.0)
leftKnee:add_child(undoleftKnee)

leftLowerLegJoint = gr.joint('leftLowerLegJoint', {-500,0,500},{-500,0,500})
undoleftKnee:add_child(leftLowerLegJoint)

leftLowerLeg = gr.mesh('cube','leftLowerLeg')
leftLowerLegJoint:add_child(leftLowerLeg)
leftLowerLeg:set_material(blue)
leftLowerLeg:scale(2,2*2.5,2)
leftLowerLeg:translate(0,-2.5,0)

undoleftLowerLeg = gr.node('undoleftLowerLeg')
undoleftLowerLeg:scale(1.0,1.0/2.5,1.0)
leftLowerLeg:add_child(undoleftLowerLeg)

leftAnkle = gr.mesh('sphere','leftAnkle')
undoleftLowerLeg:add_child(leftAnkle)
leftAnkle:set_material(white)
leftAnkle:scale(0.5,0.5,0.5)
leftAnkle:translate(0,-1.2,0)

undoleftAnkle = gr.node('undoleftAnkle')
undoleftAnkle:scale(1.0,1.0/2.5,1.0)
leftAnkle:add_child(undoleftAnkle)

leftFootJoint = gr.joint('leftFootJoint', {-500,0,500},{-500,0,500})
undoleftAnkle:add_child(leftFootJoint)

leftFoot = gr.mesh('cube','leftFoot')
leftFootJoint:add_child(leftFoot)
leftFoot:set_material(blue)
leftFoot:scale(2.0,2.0,4.0)
leftFoot:translate(0.0,-1.7,1.0)

undoleftFoot = gr.node('undoleftFoot')
undoleftFoot:scale(1.0/2.0,1.0/2.0,1.0/4.0)
leftFoot:add_child(undoleftFoot)

leftToes = gr.mesh('suzanne','leftToes')
undoleftFoot:add_child(leftToes)
leftToes:set_material(red)
leftToes:scale(1.0,2.0,1.0)
leftToes:translate(0.0,2.0,2.0)

----------------------------------------------------------------------------------------

--Shell

shellRoot = gr.node('shellRoot')
shellRoot:scale(1/0.25,1/0.25,1/0.25)
rootnode:add_child(shellRoot)

shellTorso = gr.mesh('cube', 'shellTorso')
shellRoot:add_child(shellTorso)
shellTorso:set_material(purple)
shellTorso:translate(0.0,0.0,5.0)
shellTorso:scale(0.25,0.25,0.25)
shellTorso:scale(1.0,1.0,4.0)

undoshellTorso = gr.node('undoshellTorso')
undoshellTorso:scale(1.0/0.25,1.0/0.25,1.0)
shellTorso:add_child(undoshellTorso)


shellHead = gr.mesh('suzanne', 'shellHead')
undoshellTorso:add_child(shellHead)
shellHead:set_material(red)
shellHead:scale(0.25,0.25,0.25)
shellHead:translate(0.0,0.0,0.5)

shellBackJoint1 = gr.joint('shellBackJoint1', {-500,0,500},{-500,0,500})
undoshellTorso:add_child(shellBackJoint1)
shellBackJoint1:translate(-0.1,-0.1,-0.8)
shellBackJoint1:rotate('y',-30)
shellBackJoint1:rotate('x',30)


shellBack1 = gr.mesh('sphere', 'shellBack1')
shellBackJoint1:add_child(shellBack1)
shellBack1:set_material(black)
shellBack1:scale(0.15,0.15,0.5)


shellBackJoint2 = gr.joint('shellBackJoint2', {-500,0,500},{-500,0,500})
undoshellTorso:add_child(shellBackJoint2)
shellBackJoint2:translate(-0.1,0.1,-0.8)
shellBackJoint2:rotate('y',-30)
shellBackJoint2:rotate('x',-30)


shellBack2 = gr.mesh('sphere', 'shellBack2')
shellBackJoint2:add_child(shellBack2)
shellBack2:set_material(red)
shellBack2:scale(0.15,0.15,0.5)


shellBackJoint3 = gr.joint('shellBackJoint3', {-500,0,500},{-500,0,500})
undoshellTorso:add_child(shellBackJoint3)
shellBackJoint3:translate(0.1,-0.1,-0.8)
shellBackJoint3:rotate('y',30)
shellBackJoint3:rotate('x',30)


shellBack3 = gr.mesh('sphere', 'shellBack3')
shellBackJoint3:add_child(shellBack3)
shellBack3:set_material(green)
shellBack3:scale(0.15,0.15,0.5)


shellBackJoint4 = gr.joint('shellBackJoint4', {-500,0,500},{-500,0,500})
undoshellTorso:add_child(shellBackJoint4)
shellBackJoint4:translate(0.1,0.1,-0.8)
shellBackJoint4:rotate('y',30)
shellBackJoint4:rotate('x',-30)


shellBack4 = gr.mesh('sphere', 'shellBack4')
shellBackJoint4:add_child(shellBack4)
shellBack4:set_material(blue)
shellBack4:scale(0.15,0.15,0.5)













return rootnode
