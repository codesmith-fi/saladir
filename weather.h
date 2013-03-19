/* weather.h */

#ifndef _WEATHER_H_DEFINED
#define _WEATHER_H_DEFINED


#include "saladir.h"

#include "output.h"
#include "types.h"
#include "gametime.h"
#include "options.h"


#define WEATHER_NORAIN 0
#define WEATHER_NOWIND 0

/* minimum and maximum temperatures */
#define WEATHER_TEMPRANGE 50

#define WINTER_MINTEMP -45
#define SPRING_MINTEMP -20
#define SUMMER_MINTEMP -5
#define AUTUMN_MINTEMP -12

#define WINTER_RAINPROB 20
#define SUMMER_RAINPROB 15
#define SPRING_RAINPROB 30
#define AUTUMN_RAINPROB 35

/* min and max duration of current weather conditions */
#define WEATHER_MINDURA 20
#define WEATHER_MAXDURA 60

/* maximum wind (m/s) */
#define WEATHER_MAXWIND 35

/* max rain */
#define WEATHER_MAXRAIN 10

class Weather
{
  private:

   typedef struct
   {
	 int16s temperature;
	 int16s rain;
	 int16s wind;
	 bool   raintype;  /* true = snow, false = water */

	 /* how long (minutes) does the current weather stay */
	 int16u duration;
	 int32u ticks;
   } TWeather;

   /* weather conditions */
   TWeather current;
   TWeather destination;

   bool change_temperature(Gametime & mt, bool report);
   bool change_wind(Gametime & mt, bool report);
   bool change_rain(Gametime & mt, bool report);
   void change_weather(Gametime & mt);      
   void modify(void);

  public:
   Weather();  /* constructors */
   ~Weather(); /* destructor */
   
//   Weather & operation=(const Weather & obj);
   bool Weather::load(_FH_ fh);
   bool Weather::save(_FH_ fh);

   /* change randomly according to the season */
   void passtime(int32u tickspassed, Gametime & mt);
   void report(void);
};

#endif
