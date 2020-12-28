/*Taken from slides*/
template < typename Ttrue, typename Tfalse, bool>
struct IfThenElse;

template <typename Ttrue, typename Tfalse>
struct IfThenElse < Ttrue, Tfalse, false>
{
	typedef Tfalse Type;
};

template <typename Ttrue, typename Tfalse>
struct IfThenElse < Ttrue, Tfalse, true>
{
	typedef Ttrue Type;
};