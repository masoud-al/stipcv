

package = newpackage()
package.name = "stfeatures"
package.kind = "exe"
package.language = "c++"


package.bindir="."

package.objdir = "."
package.config["Debug"].objdir   = "debug"
package.config["Release"].objdir = "release"




package.config["Debug"].defines         = { "LINUX=1", "DEBUG=1", "_DEBUG=1" };
package.config["Debug"].buildoptions    = { "-D_DEBUG -ggdb -Wall" }

package.config["Release"].defines       = { "LINUX=1", "NDEBUG=1" };
package.config["Release"].buildoptions  = { "-Wall" }


package.includepaths = { 
    "/homes/malipour/include/opencv"
}

package.libpaths = { 
    "/homes/malipour/lib/"
}
 

package.linkoptions = { "-L/homes/malipour/lib -lcxcore -lcv -lhighgui -lcvaux -lml"  }
 


package.files = { matchfiles (
    "*.h",
    "*.cpp"
    )
}
