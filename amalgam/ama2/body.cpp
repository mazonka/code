/*

mycode
*/
void main01()
{
	cout<<"hello\n";
}
#undef START
#define START main01()
/*


Now another code
*/
#undef START
#define START main02(args)
void main02(vector<string> args)
{
	cout<<"args:";
	for( auto i : args ) cout<<' '<<i;
	cout<<'\n';
}
