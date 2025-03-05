class position:
{
   public :
   int cost;
   int bennefit;
   int player;
   position(){}
   ~position(){}

};
class shipposition:
{
   public :
   int cost;
   int bennefit;
   position(){}
   ~position(){}

};
class player:
{
   public :
   int ID=0;
   std::string name="";
}