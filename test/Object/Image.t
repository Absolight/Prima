print "1..13 create,type check,paintInfo,load,palette,pixel,paint,get_paint_state(),type,pixel,stretch,set_pixel bpp1,reverse stretch,bpp4,bpp8,RGB,short,long,float\n";

my $i = Prima::Image-> create(
   width => 20,
   height => 20,
   type => im::BW,
   conversion => ict::None,
   preserveType => 1,
);


ok( $i);
ok( $i-> type == im::BW);
$i-> begin_paint_info;
ok( $i-> get_paint_state == 2);
$i-> end_paint_info;
ok( $i-> load( 'Object/rc.bmp'));
my @p = @{$i-> palette};
ok( $p[0] == 0 && $p[1] == 0 && $p[2] == 0 && $p[3] == 0xFF && $p[4] == 0 && $p[5] == 0);
ok(  $i-> get_pixel( 0,0) == 0 && $i-> get_pixel( 15,15) == 0 && $i-> get_pixel( 1,1) != 0);
$i-> begin_paint;
ok( $i-> get_paint_state == 1);
$i-> end_paint;
ok( $i-> get_paint_state == 0);
ok( $i-> type == im::bpp1);
ok( $i-> get_pixel( 0,0) == 0 && $i-> get_pixel( 15,15) == 0);
$i-> size( 160, 160);
ok( $i-> get_pixel( 0,0) == 0 && $i-> get_pixel( 159,159) == 0);
$i-> size( 16, 16);
$i-> set_pixel( 15, 15, 0xFFFFFF);
ok( $i-> get_pixel( 15,15) != 0);
$i-> size( -16, -16);
ok( $i-> get_pixel( 0,0) != 0 && $i-> get_pixel( 15,15) == 0);

my $j;
for ( im::bpp4, im::bpp8) {
   $i-> type( $_);
   $i-> palette([0xFF, 0, 0xFF]);
   $i-> set_pixel( 3, 3, 0xFF00FF);
   $j = $i-> get_pixel( 3,3);
   $i-> size( -16, -16);
   ok( $i-> get_pixel( 12,12) == 0xFF00FF && $j == 0xFF00FF);
}

for ( im::RGB, im::Short, im::Long) {
   $i-> type( $_);
   $i-> set_pixel( 3, 3, 0x1234);
   $j = $i-> get_pixel( 3,3);
   $i-> size( -16, -16);
   ok( $i-> get_pixel( 12,12) == 0x1234 && $j == 0x1234);
}

$i-> type( im::Float);
$i-> set_pixel( 3, 3, 25);
$j = $i-> get_pixel( 3,3);
$i-> size( -16, -16);
ok( abs( 25 - $i-> get_pixel( 12,12)) < 8 && abs(25-$j) < 8);


$i-> destroy;

1;
