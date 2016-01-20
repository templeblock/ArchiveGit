{if is_object($MessageCallback) && $MessageCallback->validMessage()}
{$MessageCallback->getParameter('SenderUserName')} sent you a {$MessageCallback->getParameter('MediaType')}!  Enter Code {$MessageCallback->getId()} at mixxer.com/invited to store it in your free online Mixxer account, where you can find, upload and share mobile content.
{else}
    Unexpected message
{/if}
To block, call 1-877-4MIXXER.
