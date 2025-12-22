

## Stage 1: Set up build environment based on a standard image
##############################################################

FROM ubuntu:latest AS builder

## Install build tools and dependencies
RUN apt-get update && apt-get install -y \
        build-essential g++ make

## Set working directory for the build		
##WORKDIR /app

## Copy content from host to image
COPY . .

## Build the application with make tool
RUN make clean
RUN make
	
## Stage 2: Set up runtime environment
######################################

FROM scratch AS runtime

## Set working directory for the runtime
##WORKDIR /app

## Copy the compiled executable from build stage to image
COPY --from=builder /determ_gen .

## Copy the default parameter file from the build stage
COPY --from=builder /inout/determ_config.par .

## For diagnostic purposes
##RUN pwd
##RUN ls -al /

# Default command to run the binary when the container starts
CMD ["/determ_gen", "-help"]

#############################################################################
#############################################################################
## Remark:
##
## Build image named 'determ_gen_image' with either one of these command
## line instructions:
##
## docker build -t determ_gen_image .
##
## This empties the cache (i.e. starts building completely anew) and
## displays building process (including output of RUN pwd and RUN ls -al):
##
## docker build --no-cache --progress=plain -t determ_gen_image .
##
#############################################################################
#############################################################################

	