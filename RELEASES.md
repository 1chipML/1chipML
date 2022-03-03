# Releases Guide

## Methodology

The main guiding principle in releasing a new version is always: the quality of
the master branch should never be compromised.

Below, details are provided on how to properly merge pull requests to the
repositories.

## Merge Request

When a coder feels ready to share a contribution with the community,
a pull request is sent to the maintainers.

## Reviewing Pull Requests

We plan to review every pull request directly from the GitHub website.
This allows public commentaries and provides transparency for all users.
While sharing feedback always be reasonable, civil, courteous, and kind.
Disagreement is obviously acceptable, but the conversation has to be polite
and respectful. In the case of uncivil or abusive comments, your commit
privileges will be revoked and you will be invited to leave the project.

During a review, always bear in mind the following points:

### Do this changes make sense?

In the case you do not understand the proposed modifications or what they accomplish,
always ask the author to provide clarifications, as many times as needed. For instance,
ask the author to add comments and/or clarifications to make the intentions clear.

Sometimes, such clarifications may reveal that the author is not using the
code correctly, or is unaware of features that already accommodate their needs.
In this particular case, provide explanations or a code sample
that addresses the pull request and, then, close the pull request once the
conversation with the author is finalized.

### Does this change have positive impact on the library?

Not all proposed changes will have a positive impact to the project.
Ask yourself if the proposed change is a real improvement or, rather, it
simply is a personal preference on the part of the author. All pull
requests which do not have a clear positive impact should be closed
without merging.

### Is this change really a new feature?

For any given pull request, ask yourself "is this really a new feature?".
In the positive case, does the pull request contain enough details to
justify the feature? If not, ask the author to provide that specific information.

Also, are new tests in place that can check all newly introduced features? In
the negative case, do not merge the feature until the tests are created!
The same philosophy, obviously, applies to the documention (is the new feature
documented properly?).

Finally, ask yourself if the new feature is necessary for general use cases.
In the negative case, recommend to the author to maintain the feature on his/her
own, and close the request (you may also recommend to check if this feature gains
traction among users with time, and suggest to re-submit if/when that happens).

## Merge Approval

The project maintainers include a GFM (Good For Me) in the comments on the pull
request to indicate acceptance prior to merging. A change requires "GFMs" from
two project maintainers. In the particular case that the code is written by a
maintainer, the change only requires one additional "GFM".

