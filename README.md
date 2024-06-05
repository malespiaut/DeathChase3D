# Deathchase 3D

Written by Paul Robson (ambling along slowly) in Autumn 2002. This took ages, largely because I got
fed up with it and couldn't be bothered to finish it.

Keys are Z X K M movement/speed, Space fire, ESC exit.

These can be changed in IO.C. Joystick support will be added when I get mine working.

There is no sound. Sorry.

The only required library is SDL1.2 or better, probably (:)). Of particular interest to coders may be
the file font.c which is a bitmapped font, which kind of gets scaled, but rounded as well, so it looks
angular but not blocky. A cheapo font - no TTF or data files, which can be scaled on the fly.

This game was mostly written using KDevelop 3.0 under SuSE 8.0 and later 8.1

Paul Robson (autismuk@autismuk.freeserve.co.uk)


          
Parameters can be listed via -h and allow you to fine tune the game, making it easier, harder or whatever.
Many of these are percentage values from the default. They are not validated very much, so if you set
something silly it'll crash the app, or hopefully die with an error message.

They are, in no sensible order :-

--xscreen n --yscreen n --depth n
    Set the display size and depth. Default is 800 x 600 pixels x 16 bpp.

--spritescaler n
    Number of screens used for scaled tree graphics.  Default is 4. Decreasing will make the trees jerkier
    This is because the trees are predrawn in various sizes over a set of screens. Some systems might have
    problems with this. Tho' I haven't found one yet :) OTOH, most of my systems are NVidia ones of some sort.

--scalespeed n
    Scale the speed up or down ; default is 100 as a percentage

--acceltime n
    Scale the acceleration rate up down ; default 100 as a percentage

--turningcircle n
    Scale the bike turning circle rate up down ; default 100 as a percentage

--enemynear n
    Nearest position enemy can come to you. Default is 75 (percent of distance visible)

--emin n --emax n
    Minimum and Maximum speeds of enemy bikes, as percentage of player flat out speed. defaults are 80 and 120

--mspeed n
    Missile speed ; default is 100 as a percentage

--collidepercent n
    Percentage value determines how precise collision with trees is. Default is 100, lowering it makes it easier to avoid trees

--precision n
    Same as above, but precision of hitting enemies. higher makes it easier to hit them.

--basetrees n --inctrees n [3] [1]
    Sets number of trees. Value is basetrees+(level-1)*inctrees - start level is 1

--level n
    Start level [1]

--enemies n
    Enemies per level [2]

--lives n
    Number of lives [3]


