print "1..5 onPaint message,update_view,scroll,invalid area consistency,get_pixel,clipRect,transform\n";

$dong = 0;
$w-> bring_to_front;
my @rcrect;
my $ww = $w-> insert( Widget => origin => [ 0, 0] => size => [ 8, 8],
syncPaint => 0,
buffered => 0,
cursorSize => [ 30, 30],
cursorVisible => 1,
onPaint => sub {
   $_[0]-> on_paint( $_[1]);
   $dong = 1;
   @rcrect = $_[0]-> clipRect;
});
ok( $dong || &__wait);
$dong = 0;
$ww-> repaint;
$ww-> update_view;
ok( $dong);

$dong = 0;
$ww-> scroll( 2, 2);
$ww-> update_view;
ok( $dong );

$ww-> invalidate_rect( 0, 0, 2, 2);
$ww-> update_view;
ok( $rcrect[0] == 0 && $rcrect[1] == 0 && $rcrect[2] == 1 && $rcrect[3] == 1);

$ww-> buffered(1);
$ww-> set( onPaint => sub {
   my $x = $_[1];
   $_[0]-> on_paint( $x);
   $x-> set_pixel( 0,0,cl::Black);
   ok( $x-> get_pixel(0,0) == 0);
   $x-> color( cl::White);
   $x-> bar( 0, 0, 7, 7);
   $x-> color( cl::Black);
   $x-> clipRect( 2, 2, 3, 3);
   $x-> bar( 1, 1, 2, 2);
   $x-> clipRect( 0, 0, $x-> size);
   ok( $x-> get_pixel( 2,2) == 0 && $x-> get_pixel( 1,1) == 0xFFFFFF);

   $x-> color( cl::White);
   $x-> bar( 0, 0, 7, 7);
   $x-> color( cl::Black);
   $x-> transform( -1, 1);
   $x-> bar( 2, 2, 3, 3);
   $x-> transform( 0, 0);
   ok( $x-> get_pixel( 1,4) == 0 &&
       $x-> get_pixel( 3,2) == 0xFFFFFF
   );
});
$ww-> repaint;
$ww-> update_view;

$ww-> set( onPaint => undef);

$ww-> destroy;

1;

