library ieee;
use ieee.std_logic_1164.all;

entity ROM_2 is
  port ( A : in std_logic_vector(2 downto 0);
         Q : out std_logic_vector(7 downto 0));
end entity ROM_2;

architecture Behavioral of ROM_2 is
type M is array ( 0 to 7) of std_logic_vector(7 downto 0);
constant ROM : M := (	 0  => "00000000",
								 1  => "01000110",
								 2  => "11001110",
								 3  => "10001010",
								 4  => "10010010",
								 5  => "11110010",
								 6  => "01100010",
								 7  => "00000000");
								 
begin
	process (A)
	begin
		case A is
		when "000" => Q <= ROM(0);
		when "001" => Q <= ROM(1);
		when "010" => Q <= ROM(2);
		when "011" => Q <= ROM(3);
		when "100" => Q <= ROM(4);
		when "101" => Q <= ROM(5);
		when "110" => Q <= ROM(6);
		when "111" => Q <= ROM(7);
		end case;
	end process;

end architecture Behavioral;