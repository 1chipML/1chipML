# Github Pipelines

## Presentation

These pipelines allow us to make sure that a PR does not destroy the current functionalities.

## Current Workflows

1. Workflow that verifies the solution (compilation,linting and tests)
2. Workflow that verifies the documentation and uploads the compiled latex documentation to the repository (In Developement)


## In Development

### Latex Pipeline

The latex pipeline is fully functional but we require the repository owners approval for modifying the settings of the repository. Github Actions
must be given read and write access in order to be able to upload the documentation once it is compiled. The purpose of this pipeline is to remove the need
for any contributor to compile the documentation.
