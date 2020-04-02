function myRobot = robotKinematicsFcn()

scal = 0.1;     % unit to cm

% Robot parameters
d1 = scal * 110;
d4 = scal * 5;

r1 = scal * 10;
r2 = scal * 138;
r3 = scal * 90;
r4 = scal * 80;

y_offset = scal * 20;  

alpha1 = 90 * (pi/180);

% Link = theta d a alpha
L(1) = Link([0 d1 -r1 alpha1]);
L(2) = Link([0 0 r2 0]);
L(3) = Link([0 0 r3 0]);
L(4) = Link([0 -d4 r4 0]);

myRobot = SerialLink(L, 'name', 'TigerOne');

% Add offset in the tip (a translation matrix)
myRobot.tool = transl(0, y_offset, 0);

end