// defining_member_templates_outside_class.cpp
//template<typename T>
class X
{
public:
   template<typename U>
   U mf() const;
};

//template<typename T> 
template <typename U> inline U X::mf() const { return U(); }
template <> inline int X::mf<int>() const { return 0;}

int main()
{
	X x;
	x.mf<int>();
}
