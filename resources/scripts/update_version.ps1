function ReadNumbers 
{
    if ($args.Length -lt 2)
    {
        return $null
    }

    $buffer = $null
    for ($i = 0; $i -lt $args[0].Length; $i++)
    {
        $currentChar = $args[0].Substring($i, 1)

        if (($currentChar -match "[0-9]") -or ($currentChar -eq $args[1]))
        {
            $buffer += $currentChar
        }
    }

    return $buffer
}

$version_file = $null
$resources_file = $null

for ($i = 0; $i -lt $args.Length; $i++)
{
    $args[$i] = $args[$i].ToString().ToLower()
    if (($args[$i] -eq "-resourcefile") -and (($i + 1) -le $args.Length))
    {
        $resources_file = $args[$i + 1]
        $i++
        echo "Setting resources file to '$resources_file'"
    }
    elseif (($args[$i] -eq "-versionfile") -and (($i + 1) -le $args.Length))
    {
        $version_file = $args[$i + 1]
        $i++
        echo "Setting version file to '$version_file'"
    }
    else
    {
        echo "Unknown argument: $($args[$i])!"
        exit 1
    }
}

if (($version_file -eq $null) -or ($resources_file -eq $null))
{
	echo "No resource and / or version file provided!"
	echo "Press any key to exit..."
	$host.UI.RawUI.ReadKey("NoEcho,IncludeKeyDown") >$null
	exit 1
}

# Read version file contents and filter GAME_VERSION line
$readLine = $(Get-Content $version_file | Select-String -Pattern "GAME_VERSION").ToString().Trim()

# Get version from line
$version = ReadNumbers $readLine '.'
if ($version -eq $null)
{
	echo "Couldn't read version from version file!"
	$host.UI.RawUI.ReadKey("NoEcho,IncludeKeyDown") >$null
    exit -1
}

echo "Detected new game version: $version"

# Get lines containing FILEVERSION from resources file
$fileContent = Get-Content $resources_file

# Replace version in resources file
for ($i = 0; $i -lt $fileContent.Length; $i++)
{
    $fileContent[$i] = $fileContent[$i].ToString()
    if ($(Select-String -Pattern "FILEVERSION" -InputObject $fileContent[$i]) -eq $null)
    {
        continue
    }
   
    #
    $firstNumberIndex = -1
    for ($j = 0; $j -lt $fileContent[$i].Length; $j++)
    {
        if ($fileContent[$i].Substring($j, 1) -match "[0-9]")
        {
            $firstNumberIndex = $j
            break
        }
    }

    $replacementForm = $null
    $currentNumbers = $null
    if ($fileContent[$i].StartsWith("FILEVERSION"))
    {
        # Replace file version in form 0,0,0,0
        $splitVersion = $version.ToString().Split('.')
        $replacementForm = "0,$($splitVersion[0]),$($splitVersion[1]),$($splitVersion[2])"
        $currentNumbers = ReadNumbers $fileContent[$i] ','
    }
    else
    {
        # Replace file version in form "0.0.0"
        $replacementForm = "$version"
        $currentNumbers = ReadNumbers $fileContent[$i] '.'
    }    
    
    # Replace line in file content array
    $fileContent[$i] = $fileContent[$i].Remove($firstNumberIndex, $currentNumbers.Length)
    $fileContent[$i] = $fileContent[$i].Insert($firstNumberINdex, $replacementForm)
}

# Write file contents to resources file
echo "Writing version $version to $resources_file..."

$outputStream = [System.IO.StreamWriter] "$resources_file"
for ($i = 0; $i -lt $fileContent.Length; $i++)
{
    $outputStream.WriteLine($fileContent[$i])
}
$outputStream.Close()

echo "Version updated to $version!"

# Clean up variables
rv version_file -Force
rv resources_file -Force
rv readLine -Force
rv version -Force
rv fileContent -Force
rv i -Force
rv j -Force
rv currentNumbers -Force
rv firstNumberIndex -Force
rv replacementForm -Force
rv outputStream -Force