# Define a variable for classpath
CLASS_PATH = .
JAVA_HOME=/usr/lib/jvm/java-1.8.0-openjdk-amd64/
JAVA_PKG=edu/cs300


UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
	OSFLAG := linux
	SHARED_LIB := libsystem5msg.so
	LINK_FLAGS := -shared
endif
ifeq ($(UNAME_S),Darwin)
	OSFLAG := darwin
	SHARED_LIB := libsystem5msg.dylib
	LINK_FLAGS := -dynamiclib
endif


all:
	@echo $(OSFLAG)

.SUFFIXES: .java .class

.java.class:
	javac -classpath $(CLASS_PATH) $(JAVA_PKG)/*.java

classes: $(CLASSES:.java=.class)

CLASSES = \
	$(JAVA_PKG)/PlayerStatsTracker.java \
	$(JAVA_PKG)/PlayerToDateStatsReport.java \
	$(JAVA_PKG)/StatsObj.java \
	$(JAVA_PKG)/Worker.java \
	$(JAVA_PKG)/AtBatPitchResult.java \
	$(JAVA_PKG)/MessageJNI.java \
	$(JAVA_PKG)/DebugLog.java

classes: $(CLASSES:.java=.class)


all : edu_cs300_MessageJNI.h gameplaytracker $(JAVA_PKG)/PlayerStatsTracker.class $(SHARED_LIB) 

edu_cs300_MessageJNI.h: edu/cs300/MessageJNI.java
	javac -h . edu/cs300/MessageJNI.java
    
gameplaytracker:gameplaytracker.c stats_record_formats.h
	gcc -std=c99 -D_GNU_SOURCE $(MAC_FLAG) gameplaytracker.c -o gameplaytracker

$(JAVA_PKG)/PlayerStatsTracker.class: $(JAVA_PKG)/PlayerStatsTracker.java $(JAVA_PKG)/Worker.java $(JAVA_PKG)/AtBatPitchResults.java  $(JAVA_PKG)/PlayerToDateStatsReport.java  $(JAVA_PKG)/StatsObj.java
	javac -classpath $(CLASS_PATH) $(JAVA_PKG)/*.java

edu_cs300_MessageJNI.o:stats_record_formats.h edu_cs300_MessageJNI.h system5_msg.c queue_ids.h
	gcc -c -fPIC -I${JAVA_HOME}/include -I${JAVA_HOME}/include/$(OSFLAG) -D$(OSFLAG) system5_msg.c -o edu_cs300_MessageJNI.o

$(SHARED_LIB):stats_record_formats.h edu_cs300_MessageJNI.h edu_cs300_MessageJNI.o
	gcc $(LINK_FLAGS) -o $(SHARED_LIB) edu_cs300_MessageJNI.o -lc

test: msgsnd msgrcv edu/cs300/MessageJNI.class $(SHARED_LIB)
	./msgsnd
	java -cp . -Djava.library.path=. edu.cs300.MessageJNI
	./msgrcv


msgsnd: msgsnd_atbat.c stats_record_formats.h queue_ids.h
	gcc -std=c99 -D_GNU_SOURCE -D$(OSFLAG) msgsnd_atbat.c -o msgsnd

msgrcv: msgrcv_stats.c stats_record_formats.h queue_ids.h
	gcc -std=c99 -D_GNU_SOURCE msgrcv_stats.c -o msgrcv


clean :
	rm *.o $(SHARED_LIB) edu_cs300_MessageJNI.h $(JAVA_PKG)/*.class gameplaytracker msgsnd msgrcv