use strict;
use Carp;
use Prima::Classes;
use Prima::Buttons;

$::application = Prima::Application-> create( name => "Generic.pm");
my $w = Prima::Window-> create(
   name       => "D1",
   text    => "Window Number One",
   origin     =>  [ 100, 300],
   borderStyle=> bs::Sizeable,
   size       =>  [ 350, 100],
   backColor  => cl::Green,
   popupItems => [["Change owner"=> sub { $_[0]-> popup-> owner (( $_[0]-> name eq "D1") ? $::application-> D2 : $::application-> D1); }]],
   menuItems  => [["Change owner"=> sub { $_[0]-> menu-> owner (( $_[0]-> name eq "D1") ? $::application-> D2 : $::application-> D1); }]],
   onTimer    => sub { $_[0]->backColor(($_[0]->backColor == cl::Green) ? cl::LightGreen : cl::Green)},
);

my $w2 = Prima::Window-> create(
   name      => "D2",
   text   => "Window Number Two",
   origin    =>  [ 500, 300],
   size      =>  [ 450, 200],
   font      => { name=>"System VIO",size=>18},
   backColor => cl::Yellow,
   onTimer   => sub { $_[0]->backColor(($_[0]->backColor == cl::Yellow) ? cl::White : cl::Yellow)},
);

$w-> insert( Button =>
   rect => [ 10 ,10, 50, 30],
   text => "<",
   font => { height => 18},
   onClick => sub { $_[0]-> owner-> borderIcons(bi::Minimize|bi::TitleBar)},
);
$w-> insert( Button =>
   rect => [ 60 , 10, 100, 30],
   text => ">",
   font => { height => 18},
   onClick => sub { $_[0]-> owner-> borderIcons(bi::TitleBar|bi::SystemMenu|bi::Minimize|bi::Maximize)},
);


$w-> insert( Button =>
   growMode => gm::Center,
   text  => "Change owner",
   onClick  => sub {
      my $oldOwner = $_[0]-> owner;
      $_[0]-> owner (( $_[0]-> owner-> name eq "D1") ? $::application-> D2 : $::application-> D1);
      my $timer = $::application-> Timer1;
      if ( $timer-> {win} == $w)
      {
         $timer-> {win} = $w2;
      } else {
         $timer-> {win} = $w;
      }
   },
);

$::application-> insert( Timer =>
   timeout  => 1000,
   name     => Timer1 =>
   onCreate => sub { $_[0]-> start; $_[0]-> {win} = $w; },
   onTick   => sub {
      return unless $_[0]-> {win}-> alive;
      $_[0]-> {win}-> backColor( $_[0]-> {win}-> backColor ^ 0x00FFFFFF);
   },
);

run Prima;

